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

	ContactList::instance()->addHostState(m_host, ContactList::CONNECTING);
	m_connection = new Connection(Connection::STATUS, this);
	m_connection->setVerbose(ID::NONE);
	ContactList::instance()->addSignals(m_connection);
	QObject::connect(m_connection.data(), &Connection::connected, m_connection.data(), &Connection::disconnect);
	QObject::connect(m_connection.data(), &Connection::connected, this, &PingClient::onConnected);
	QObject::connect(m_connection.data(), &Connection::connectFailed, this, &PingClient::onError);
	QObject::connect(m_connection.data(), &Connection::socketError, this, &PingClient::onError);
	m_connection->connect(m_host);
}

void PingClient::onConnected()
{
	ContactList::instance()->removeHostState(m_host, ContactList::CONNECTING);
}
void PingClient::onError(QString error)
{
	ContactList::instance()->removeHostState(m_host, ContactList::CONNECTING);
}

