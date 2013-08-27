#include "chatclient.h"
#include "connection.h"
#include "contactlist.h"
#include "config.h"

ChatClient::ChatClient(const Contact& contact, QObject* parent)
		: QObject(parent), m_host(contact.host()), m_contact(contact), m_connection(0), m_messagequeue(),
			m_messagequeue_mutex()
{
}
ChatClient::ChatClient(const Host& host, QObject* parent)
		: QObject(parent), m_host(host), m_contact(), m_connection(0), m_messagequeue(), m_messagequeue_mutex()
{
}
ChatClient::ChatClient(Connection* connection, QObject* parent)
		: QObject(parent), m_host(connection->host()), m_contact(connection->contact()), m_connection(connection),
			m_messagequeue(), m_messagequeue_mutex()
{
}
QString ChatClient::id() const
{
	return "ChatClient<" + (m_contact != Contact::INVALID_CONTACT ? m_contact.id() : m_host.id()) + ">";
}

void ChatClient::connect()
{
	if (m_connection) {
		if (!m_connection->isConnected()) {
			return;
		}
		delete m_connection;
	}

	m_connection = new Connection(Connection::CHAT, this);
	QObject::connect(m_connection, &Connection::contactFound, ContactList::instance(), &ContactList::addContact);
	QObject::connect(m_connection, &Connection::hostOnline, ContactList::instance(), &ContactList::setHostOnline);
	QObject::connect(m_connection, &Connection::hostOffline, ContactList::instance(), &ContactList::setHostOffline);
	QObject::connect(m_connection, &Connection::connected, this, &ChatClient::onConnected);
	QObject::connect(m_connection, &Connection::connected, this, &ChatClient::flush);
	QObject::connect(m_connection, &Connection::readyRead, this, &ChatClient::onReadyRead);
	QObject::connect(m_connection, &Connection::disconnected, this, &ChatClient::onDisconnected);
	QObject::connect(m_connection, &Connection::socketError, this, &ChatClient::onSocketError);
	QObject::connect(m_connection, &Connection::connectFailed, this, &ChatClient::onConnectFailed);
	m_connection->connect(m_host);
}

void ChatClient::sendMessage(QString msg)
{
	QMutexLocker locker(&m_messagequeue_mutex);
	m_messagequeue.append(msg);
}

void ChatClient::flush()
{
	log.debug("flush()");
	if (m_connection->isConnected()) {
		QTextStream stream(m_connection->socket());

		// write
		{
			QMutexLocker locker(&m_messagequeue_mutex);
			foreach (QString msg, m_messagequeue)
			{
				stream << msg << endl;
			}
			m_messagequeue.clear();
		}

	} else {
		QTimer::singleShot(0, this, SLOT(connect()));
	}
}

void ChatClient::onReadyRead()
{
	log.debug("onReadyRead()");
	QTextStream stream(m_connection->socket());

	// read
	while (m_connection->socket()->canReadLine()) {
		QString line = stream.readLine().trimmed();
		emit receivedMessage(line);
	}
}
void ChatClient::onConnected()
{
	log.debug("onConnected()");
	flush();
}

void ChatClient::onDisconnected()
{
	log.debug("onDisconnected()");
	checkForErrors();
}
void ChatClient::onSocketError(QString error, Host host)
{
	log.debug("onSocketError(%1, %2)", error, Log::print(host));
	checkForErrors();
}
void ChatClient::onConnectFailed(QString error, Host host)
{
	log.debug("onConnectFailed(%1, %2)", error, Log::print(host));
	checkForErrors();
}
void ChatClient::checkForErrors()
{
	QMutexLocker locker(&m_messagequeue_mutex);
	foreach (QString msg, m_messagequeue)
	{
		log.debug("Could not send message: %1", msg);
		emit sendMessageFailed(msg);
	}
	m_messagequeue.clear();
}
