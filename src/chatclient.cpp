#include "chatclient.h"
#include "connection.h"
#include "contactlist.h"

ChatClient::ChatClient(const Contact& contact, QObject* parent)
		: QObject(parent), m_host(contact.host()), m_contact(contact), m_connection(0), messagequeue(),
			m_messagequeue_mutex()
{
}
ChatClient::ChatClient(const Host& host, QObject* parent)
		: QObject(parent), m_host(host), m_contact(), m_connection(0), messagequeue(), m_messagequeue_mutex()
{
}
ChatClient::ChatClient(const Connection& connection, QObject* parent)
		: QObject(parent), m_host(connection.host()), m_contact(connection.contact()),
			m_connection(connection), messagequeue(), m_messagequeue_mutex()
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
	m_connection->connect(m_host);
	QObject::connect(m_connection, &Connection::contactFound, ContactList::instance(),
			&ContactList::addContact);
	QObject::connect(m_connection, &Connection::hostOnline, ContactList::instance(),
			&ContactList::setHostOnline);
	QObject::connect(m_connection, &Connection::hostOffline, ContactList::instance(),
			&ContactList::setHostOffline);
	QObject::connect(m_connection, &Connection::connected, this, &ChatClient::onConnected);
}

void ChatClient::sendMessage(QString msg)
{

}

void sync();
void onConnected();
