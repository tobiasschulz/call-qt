#include <QTcpSocket>
#include <QSettings>
#include <QTimer>

#include "call.h"
#include "maingui.h"
#include "connection.h"
#include "contactlist.h"
#include "thread.h"
#include "config.h"

QHash<Contact, Call*> Call::m_instances;

Call* Call::instance(const Contact& contact)
{
	static QMutex mutex;
	mutex.lock();
	if (!m_instances.contains(contact)) {
		m_instances[contact] = new Call(contact);
		static int num = 0;
		Thread* thread = new Thread(QString("Call<%1>").arg(++num));
		thread->start();
		m_instances[contact]->moveToThread(thread);
	}
	mutex.unlock();
	return m_instances[contact];
}

Call::Call(const Contact& contact, QObject* parent)
		: QObject(parent), m_host(contact.host()), m_contact(contact), m_connection(0), m_inputaudio(0),
			m_outputaudio(0), m_inputinfo(0), m_outputinfo(0)
{
	QObject::connect(this, &Call::started, Main::instance(), &Main::showStats);
	QObject::connect(this, &Call::stopped, Main::instance(), &Main::hideStats);
	QObject::connect(this, &Call::statsDurationInput, Main::instance(), &Main::onStatsDurationInput);
	QObject::connect(this, &Call::statsDurationOutput, Main::instance(), &Main::onStatsDurationOutput);
	QObject::connect(this, &Call::statsLatencyInput, Main::instance(), &Main::onStatsLatencyInput);
	QObject::connect(this, &Call::statsLatencyOutput, Main::instance(), &Main::onStatsLatencyOutput);
}

QString Call::id() const
{
	return "Call<"
			+ (m_contact != Contact::INVALID_CONTACT ?
					m_contact.id() : (m_host != Host::INVALID_HOST ? m_host.id() : "?")) + ">";
}
QString Call::print(PrintFormat format) const
{
	QString data =
			m_contact != Contact::INVALID_CONTACT ?
					m_contact.print(ID::PRINT_ONLY_DATA) : m_host.print(ID::PRINT_ONLY_DATA);

	if (format == PRINT_ONLY_NAME)
		return "Call";
	else if (format == PRINT_ONLY_DATA)
		return data;
	else
		return "Call " + data;
}

void Call::open()
{
	close();

	m_connection = new Connection(Connection::CALL, this);
	prepareConnection();
	m_connection->connect(m_host);
}

void Call::open(Connection* connection)
{
	close();

	m_connection = connection;
	m_connection->setParent(this);
	prepareConnection();
	if (m_connection->isConnected()) {
		QTimer::singleShot(0, this, SLOT(onConnected()));
	}
}

void Call::close()
{
	log.debug("close()");
	if (m_connection) {
		if (m_connection->isConnected()) {
			QTimer::singleShot(0, m_connection, SLOT(disconnect()));
		}
		m_connection.clear();
	}
	if (m_inputaudio) {
		m_inputaudio->stop();
		QTimer::singleShot(0, m_inputaudio, SLOT(deleteLater()));
		m_inputaudio.clear();
	}
	if (m_outputaudio) {
		m_outputaudio->stop();
		QTimer::singleShot(0, m_outputaudio, SLOT(deleteLater()));
		m_outputaudio.clear();
	}
	if (m_inputinfo) {
		m_inputinfo->stop();
		QTimer::singleShot(0, m_inputinfo, SLOT(deleteLater()));
		m_inputinfo.clear();
	}
	if (m_outputinfo) {
		m_outputinfo->stop();
		QTimer::singleShot(0, m_outputinfo, SLOT(deleteLater()));
		m_outputinfo.clear();
	}
	emit stopped();
}

void Call::prepareConnection()
{
	ContactList::instance()->addSignals(m_connection);
	QObject::connect(m_connection.data(), &Connection::connected, this, &Call::onConnected);
	QObject::connect(m_connection.data(), &Connection::disconnected, this, &Call::close);
	QObject::connect(m_connection.data(), &Connection::socketError, this, &Call::onSocketError);
	QObject::connect(m_connection.data(), &Connection::connectFailed, this, &Call::onConnectFailed);
}

void Call::setConnection(Connection* connection)
{
	close();
	open(connection);
}

void Call::onConnected()
{
	log.debug("onConnected()");

	QAudioDeviceInfo indevice = Config::instance()->currentMicrophone();
	QAudioDeviceInfo outdevice = Config::instance()->currentSpeaker();

	QAudioFormat inputformat = Config::instance()->currentAudioFormat();
	QAudioFormat outputformat = Config::instance()->currentAudioFormat();

	m_inputaudio = new QAudioInput(indevice, inputformat, this);
	log.debug("audioinput: device = %1, format = %2", Log::print(indevice), Log::print(m_inputaudio->format()));
	m_outputaudio = new QAudioOutput(outdevice, outputformat, this);
	log.debug("audiooutput: device = %1, format = %2", Log::print(outdevice), Log::print(m_outputaudio->format()));

	m_inputinfo = new AudioInfo(m_connection->socket(), inputformat, this);
	m_outputinfo = new AudioInfo(m_connection->socket(), outputformat, this);
	m_inputinfo->start();
	m_outputinfo->start();
	//m_infooutput = new AudioInfo(m_connection->socket(), format, this);
	//m_audioinput->start(m_connection->socket());
	//m_audiooutput->start(m_connection->socket());
	m_inputaudio->start(m_inputinfo);
	m_outputaudio->start(m_outputinfo);

	QObject::connect(m_inputaudio, SIGNAL(stateChanged(QAudio::State)), SLOT(handleStateChanged(QAudio::State)));
	QObject::connect(m_outputaudio, SIGNAL(stateChanged(QAudio::State)), SLOT(handleStateChanged(QAudio::State)));

	QSettings settings;
	bool showStats = settings.value("window/show-stats", true).toBool();
	if (showStats) {
		QObject::connect(m_inputaudio.data(), &QAudioInput::notify, this, &Call::notifiedInput);
		QObject::connect(m_outputaudio.data(), &QAudioOutput::notify, this, &Call::notifiedOutput);
		m_inputinfo->setLevelUpdates(true);
		m_outputinfo->setLevelUpdates(true);
		QObject::connect(m_inputinfo.data(), &AudioInfo::levelUpdated, Main::instance(), &Main::onStatsLevelInput);
		QObject::connect(m_outputinfo.data(), &AudioInfo::levelUpdated, Main::instance(), &Main::onStatsLevelOutput);
	}

	emit started();
}

void Call::notifiedInput()
{
	long bytesready = m_inputaudio->bytesReady();
	long elapsedmsec = (m_inputaudio->elapsedUSecs() / 1000.0);
	long processedmsec = (m_inputaudio->processedUSecs() / 1000.0);
	long latency = elapsedmsec - processedmsec;

	static long latencyAverage = 0;
	static long lastElapsedmsec = 0;
	if (elapsedmsec < lastElapsedmsec)
		latencyAverage = 0;
	if (latencyAverage > 2)
		latencyAverage = (latencyAverage + latency) / 2;
	else
		latencyAverage = latency;
	lastElapsedmsec = elapsedmsec;

	emit statsDurationInput(elapsedmsec);
	emit statsLatencyInput(latencyAverage);

	log.debug("microphone: bytesReady = %1, elapsedSeconds = %2, latency = %3", (int) bytesready, (int) elapsedmsec,
			(int) latency);
}

void Call::notifiedOutput()
{
	long elapsedmsec = (m_outputaudio->elapsedUSecs() / 1000.0);
	long processedmsec = (m_outputaudio->processedUSecs() / 1000.0);
	long latency = elapsedmsec - processedmsec;

	static long latencyAverage = 0;
	static long lastElapsedmsec = 0;
	if (elapsedmsec < lastElapsedmsec)
		latencyAverage = 0;
	if (latencyAverage > 2)
		latencyAverage = (latencyAverage + latency) / 2;
	else
		latencyAverage = latency;
	lastElapsedmsec = elapsedmsec;

	emit statsDurationOutput(elapsedmsec);
	emit statsLatencyOutput(latencyAverage);

	log.debug("speaker: bytesReady = %1, elapsedSeconds = %2, latency = %3", "?", (int) elapsedmsec, (int) latency);
}

void Call::handleStateChanged(QAudio::State state)
{
	log.debug("state = %1",
			state == QAudio::ActiveState ? "QAudio::ActiveState" :
			state == QAudio::StoppedState ? "QAudio::StoppedState" :
			state == QAudio::IdleState ? "QAudio::IdleState" : "unknown");
	if (state == QAudio::StoppedState) {
		close();
	}
}

void Call::onSocketError(QString error, Host host)
{
	log.debug("onSocketError(%1, %2)", error, Log::print(host));
	close();
}

void Call::onConnectFailed(QString error, Host host)
{
	log.debug("onConnectFailed(%1, %2)", error, Log::print(host));
	close();
}
