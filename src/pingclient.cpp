#include "pingclient.h"
#include "connection.h"
#include "contactlist.h"

PingClient::PingClient(const Contact& contact, QObject* parent)
		: QObject(parent), m_host(contact.host()), m_connection(0)
{
}
PingClient::PingClient(const Host& host, QObject* parent)
		: QObject(parent), m_host(host), m_connection(0)
{
}
QString PingClient::id() const
{
	return "PingClient<" + m_host.id() + ">";
}

void PingClient::ping()
{
	if (m_connection) {
		if (m_connection->isConnected()) {
			return;
		}
		m_connection->disconnect();
		m_connection = 0;
	}

	m_connection = new Connection(Connection::STATUS, this);
	ContactList::instance()->addSignals(m_connection);
	QObject::connect(m_connection.data(), &Connection::connected, m_connection.data(), &Connection::disconnect);
	m_connection->connect(m_host);
}

