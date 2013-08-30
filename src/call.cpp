#include <QTcpSocket>
#include <QTimer>

#include "call.h"
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
		: QObject(parent), m_host(contact.host()), m_contact(contact), m_connection(0), m_audioinput(0),
			m_audiooutput(0)
{
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
	if (m_audioinput) {
		m_audioinput->stop();
		QTimer::singleShot(0, m_audioinput, SLOT(deleteLater()));
		m_audioinput.clear();
	}
	if (m_audiooutput) {
		m_audiooutput->stop();
		QTimer::singleShot(0, m_audiooutput, SLOT(deleteLater()));
		m_audiooutput.clear();
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

	QAudioDeviceInfo indevice = Config::instance()->currentAudioInputDevice();
	QAudioDeviceInfo outdevice = Config::instance()->currentAudioOutputDevice();
	QAudioFormat format = Config::instance()->currentAudioFormat();
	m_audioinput = new QAudioInput(indevice, format, this);
	log.debug("audioinput: device = %1, format = %2", Log::print(indevice), Log::print(m_audioinput->format()));
	m_audiooutput = new QAudioOutput(outdevice, format, this);
	log.debug("audiooutput: device = %1, format = %2", Log::print(outdevice), Log::print(m_audiooutput->format()));
	m_audioinput->start(m_connection->socket());
	m_audiooutput->start(m_connection->socket());

	QObject::connect(m_audioinput, SIGNAL(notify()), SLOT(notified()));
	QObject::connect(m_audioinput, SIGNAL(stateChanged(QAudio::State)), SLOT(handleStateChanged(QAudio::State)));
	QObject::connect(m_audiooutput, SIGNAL(notify()), SLOT(notified()));
	QObject::connect(m_audiooutput, SIGNAL(stateChanged(QAudio::State)), SLOT(handleStateChanged(QAudio::State)));

	emit started();
}

void Call::notified()
{
	log.debug("bytesReady = %1, elapsedSeconds = %2, processedSeconds = %3", m_audioinput->bytesReady(),
			(m_audioinput->elapsedUSecs() / 1000000.0), (m_audioinput->processedUSecs() / 1000000.0));
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
