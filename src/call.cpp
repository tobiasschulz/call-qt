#include <QTcpSocket>
#include <QSettings>
#include <QTimer>

#include "call.h"
#include "maingui.h"
#include "connection.h"
#include "contactlist.h"
#include "networkutil.h"
#include "thread.h"
#include "config.h"

QHash<Contact, Call*> Call::m_instances;

Call* Call::instance(const Contact& contact)
{
	// ein call wird zwei verschiedenen thread szugewiesen und von der normalen und der looopback connection
	// verwendet und dnan wohl auch nach open() direkt geclose()ed
	static QMutex mutex;
	QMutexLocker locker(&mutex);
	if (!m_instances.contains(contact)) {
		m_instances[contact] = new Call(contact);
		static int num = 0;
		Thread* thread = new Thread(QString("Call<%1>").arg(++num));
		thread->start();
		m_instances[contact]->moveToThread(thread);
	}
	return m_instances[contact];
}

Call::Call(const Contact& contact, QObject* parent)
		: QObject(parent), m_host(contact.host()), m_contact(contact), m_connection(0), m_inputaudio(0),
			m_outputaudio(0), m_inputinfo(0), m_outputinfo(0), m_state(CLOSED), m_openlock(), m_closelock()
{
	if (!m_host.isLoopback()) {
		// for normal connections
		QObject::connect(this, &Call::started, Main::instance(), &Main::showStats);
		QObject::connect(this, &Call::stopped, Main::instance(), &Main::hideStats);
		QObject::connect(this, &Call::statsContact, Main::instance(), &Main::onStatsContact);
		QObject::connect(this, &Call::statsDurationInput, Main::instance(), &Main::onStatsDurationInput);
		QObject::connect(this, &Call::statsDurationOutput, Main::instance(), &Main::onStatsDurationOutput);
		QObject::connect(this, &Call::statsLatencyInput, Main::instance(), &Main::onStatsLatencyInput);
		QObject::connect(this, &Call::statsLatencyOutput, Main::instance(), &Main::onStatsLatencyOutput);
		QObject::connect(this, &Call::statsFormatInput, Main::instance(), &Main::onStatsFormatInput);
		QObject::connect(this, &Call::statsFormatOutput, Main::instance(), &Main::onStatsFormatOutput);
		QObject::connect(Main::instance(), &Main::volumeChangedInput, this, &Call::onVolumeChangedInput);
		QObject::connect(Main::instance(), &Main::volumeChangedOutput, this, &Call::onVolumeChangedOutput);

	} else {
		// for loopback connections
		QObject::connect(Main::instance(), &Main::volumeChangedOutput, this, &Call::onVolumeChangedInput);
		QObject::connect(Main::instance(), &Main::volumeChangedInput, this, &Call::onVolumeChangedOutput);
	}
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
	QMutexLocker locker(&m_openlock);
	if (m_state == CLOSED) {
		log.debug("open call: %1", Log::print(m_host));
		m_state = OPENING;
		close();
		m_state = OPEN;

		m_connection = new Connection(Connection::CALL, this);
		prepareConnection();
		m_connection->connect(m_host);
	} else {
		log.debug("Cannot open call: call is already active!");
	}
}

void Call::open(Connection* connection)
{
	QMutexLocker locker(&m_openlock);
	if (m_state == CLOSED) {
		log.debug("open call: %1", Log::print(connection));
		m_state = OPENING;
		close();
		m_state = OPEN;

		m_connection = connection;
		m_connection->setParent(this);
		prepareConnection();
		if (m_connection->isConnected()) {
			QTimer::singleShot(0, this, SLOT(onConnected()));
		}
	} else {
		log.debug("Cannot open call: call is already active!");
	}
}

void Call::close()
{
	if (m_closelock.tryLock(100))
		m_closelock.unlock();
	else
		return;

	QMutexLocker locker(&m_closelock);
	if (m_state == OPEN)
		log.debug("close call.");
	else if (m_state == CLOSED)
		log.debug("making sure call is closed.");

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

	if (m_state == OPEN)
		m_state = CLOSED;
	emit stopped();
}

void Call::prepareConnection()
{
	ContactList::addSignals(m_connection);
	QObject::connect(m_connection.data(), &Connection::connected, this, &Call::onConnected);
	QObject::connect(m_connection.data(), &Connection::disconnected, this, &Call::close);
	QObject::connect(m_connection.data(), &Connection::socketError, this, &Call::onSocketError);
	QObject::connect(m_connection.data(), &Connection::connectFailed, this, &Call::onConnectFailed);
}

void Call::setConnection(Connection* connection)
{
	open(connection);
}

void Call::onConnected()
{
	log.debug("onConnected()");

	QAudioDeviceInfo indevice = Config::instance()->currentMicrophone();
	QAudioDeviceInfo outdevice = Config::instance()->currentSpeaker();

	QAudioFormat inputformat = Config::instance()->currentAudioFormat();
	QAudioFormat outputformat = NetworkUtil::instance()->readAudioFormat(m_connection->headers());

	m_inputaudio = new QAudioInput(indevice, inputformat, this);
	log.debug("audioinput: device = %1, format = %2", Log::print(indevice), Log::print(m_inputaudio->format()));
	m_outputaudio = new QAudioOutput(outdevice, outputformat, this);
	log.debug("audiooutput: device = %1, format = %2", Log::print(outdevice), Log::print(m_outputaudio->format()));

	m_inputinfo = new AudioInfo(m_connection->socket(), inputformat, "microphone", this);
	m_outputinfo = new AudioInfo(m_connection->socket(), outputformat, "speaker", this);
	m_inputinfo->start();
	m_outputinfo->start();
	//m_infooutput = new AudioInfo(m_connection->socket(), format, this);
	//m_audioinput->start(m_connection->socket());
	//m_audiooutput->start(m_connection->socket());
	m_inputaudio->start(m_inputinfo);
	m_outputaudio->start(m_outputinfo);
	QObject::connect(m_connection->socket(), &QTcpSocket::readyRead, this, &Call::startSpeaker);

	QObject::connect(m_inputaudio, SIGNAL(stateChanged(QAudio::State)), SLOT(handleInputStateChanged(QAudio::State)));
	QObject::connect(m_outputaudio, SIGNAL(stateChanged(QAudio::State)), SLOT(handleOutputStateChanged(QAudio::State)));

	if (!m_host.isLoopback()) {
		QSettings settings;
		bool showStats = settings.value("window/show-stats", true).toBool();
		if (showStats) {
			QObject::connect(m_inputaudio.data(), &QAudioInput::notify, this, &Call::notifiedInput);
			QObject::connect(m_outputaudio.data(), &QAudioOutput::notify, this, &Call::notifiedOutput);
			m_inputinfo->setUpdateLevel(true);
			m_outputinfo->setUpdateLevel(true);
			QObject::connect(m_inputinfo.data(), &AudioInfo::levelUpdated, Main::instance(), &Main::onStatsLevelInput);
			QObject::connect(m_outputinfo.data(), &AudioInfo::levelUpdated, Main::instance(),
					&Main::onStatsLevelOutput);
		}
		emit statsContact(m_connection->contact());
		emit statsFormatInput(inputformat);
		emit statsFormatOutput(outputformat);
	}

	emit started();
}

void Call::startSpeaker()
{
	QObject::disconnect(m_connection->socket(), &QTcpSocket::readyRead, this, &Call::startSpeaker);
	//m_outputaudio->start(m_outputinfo);
}

void Call::onVolumeChangedInput(qreal volume)
{
	m_inputinfo->setVolume(volume);
}
void Call::onVolumeChangedOutput(qreal volume)
{
	m_outputinfo->setVolume(volume);
}

void Call::notifiedInput()
{
	long bytesready = m_inputaudio->bytesReady();
	long elapsedmsec = (m_inputaudio->elapsedUSecs() / 1000.0);
	long processedmsec = m_inputinfo->processedMilliseconds();
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

	log.debug("microphone: bytes ready = %1, elapsed ms = %2, processed ms = %3, latency = %4", (int) bytesready,
			(int) elapsedmsec, (int) processedmsec, (int) latency);
	if (m_connection) {
		log.debug("socket: bytesAvailable = %1", m_connection->socket()->bytesAvailable());
		log.debug("socket: bytesToWrite = %1", m_connection->socket()->bytesToWrite());
	}
}

void Call::notifiedOutput()
{
	long bytesready = m_connection ? m_connection->socket()->bytesAvailable() : -1;
	long elapsedmsec = (m_outputaudio->elapsedUSecs() / 1000.0);
	long processedmsec = m_outputinfo->processedMilliseconds();
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

	log.debug("speaker: bytes ready = %1, elapsed ms = %2, processed ms = %3, latency = %4", (int) bytesready,
			(int) elapsedmsec, (int) processedmsec, (int) latency);
}

void Call::handleInputStateChanged(QAudio::State state)
{
	printStates();
	if (state == QAudio::StoppedState) {
		close();
	} else if (state == QAudio::IdleState) {
		//m_inputaudio->start(m_inputaudio->start());
	}
}

void Call::handleOutputStateChanged(QAudio::State state)
{
	printStates();
	if (state == QAudio::StoppedState) {
		close();
	} else if (state == QAudio::IdleState) {
		QTimer::singleShot(250, this, SLOT(restartSpeaker()));
	}
}

void Call::restartSpeaker()
{
	if (m_outputinfo && m_outputaudio)
		m_outputaudio->start(m_outputinfo);
}

void Call::printStates()
{
	if (m_inputaudio) {
		QAudio::State state = m_inputaudio->state();
		log.debug("microphone: state = %1",
				state == QAudio::ActiveState ? "QAudio::ActiveState" :
				state == QAudio::StoppedState ? "QAudio::StoppedState" :
				state == QAudio::IdleState ? "QAudio::IdleState" : "unknown");
	}
	if (m_outputaudio) {
		QAudio::State state = m_outputaudio->state();
		log.debug("speaker: state = %1",
				state == QAudio::ActiveState ? "QAudio::ActiveState" :
				state == QAudio::StoppedState ? "QAudio::StoppedState" :
				state == QAudio::IdleState ? "QAudio::IdleState" : "unknown");
	}

	if (m_inputaudio) {
		QAudio::Error error = m_inputaudio->error();
		if (error != QAudio::NoError)
			log.error("microphone: error = %1",
					error == QAudio::OpenError ? "QAudio::OpenError" : error == QAudio::IOError ? "QAudio::IOError" :
					error == QAudio::UnderrunError ? "QAudio::UnderrunError" :
					QAudio::FatalError ? "QAudio::FatalError" : "unknown");
	}
	if (m_outputaudio) {
		QAudio::Error error = m_outputaudio->error();
		if (error != QAudio::NoError)
			log.error("speaker: error = %1",
					error == QAudio::OpenError ? "QAudio::OpenError" : error == QAudio::IOError ? "QAudio::IOError" :
					error == QAudio::UnderrunError ? "QAudio::UnderrunError" :
					QAudio::FatalError ? "QAudio::FatalError" : "unknown");
	}

	if (m_connection) {
		log.debug("socket: bytesAvailable = %1", m_connection->socket()->bytesAvailable());
		log.debug("socket: bytesToWrite = %1", m_connection->socket()->bytesToWrite());
	}

}

void Call::onSocketError(QString error, Host host)
{
	log.warn("onSocketError(%1, %2)", error, Log::print(host));
	close();
}

void Call::onConnectFailed(QString error, Host host)
{
	log.error("onConnectFailed(%1, %2)", error, Log::print(host));
	close();
}
