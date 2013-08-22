#include "pingclient.h"
#include "connection.h"
#include "contactlist.h"

PingClient::PingClient(const Contact& contact, QObject* parent)
		: QObject(parent), m_host(contact.host()), m_connection(0) {
}
PingClient::PingClient(const Host& host, QObject* parent)
		: QObject(parent), m_host(host), m_connection(0) {
}
QString PingClient::id() const {
	return "PingClient<" + m_host.id() + ">";
}

void PingClient::ping() {
	if (m_connection) {
		if (!m_connection->isConnected()) {
			return;
		}
		delete m_connection;
	}

	m_connection = new Connection(Connection::STATUS, this);
	m_connection->connect(m_host);
	QObject::connect(m_connection, &Connection::contactFound, ContactList::instance(), &ContactList::addContact);
	QObject::connect(m_connection, &Connection::hostOnline, ContactList::instance(), &ContactList::setHostOnline);
	QObject::connect(m_connection, &Connection::hostOffline, ContactList::instance(), &ContactList::setHostOffline);
	QObject::connect(m_connection, &Connection::connected, m_connection, &Connection::close);
}

