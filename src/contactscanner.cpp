#include "QtGlobal"

#include "contactscanner.h"
#include "statusconnection.h"
#include "contactlist.h"
#include "config.h"
#include "util.h"

ContactScanner::ContactScanner(ContactList* contacts, QObject* parent)
		: QThread(parent), m_contacts(contacts) {
}
QString ContactScanner::id() const {
	return "ContactScanner";
}

void ContactScanner::run() {
	log.debug("ContactScanner::run()");

	foreach (const QString & hostname, Config::hosts_to_contact())
	{
		StatusConnection* connection;
		if (m_connections.contains(hostname)) {
			connection = m_connections[hostname];
		} else {
			QHostAddress hostaddr = NetworkUtil::parseHost(hostname);
			connection = new StatusConnection(hostaddr, Config::DEFAULT_PORT);
			QObject::connect(connection, &StatusConnection::readyRead, this, &ContactScanner::readyRead);
			m_connections[hostname] = connection;
		}

		log.debug("connection: %1, state: %2", connection->id(), connection->isConnected() ? "true" : "false");
	}

	exec();
}

void ContactScanner::readyRead() {
	log.debug("readyRead()");
}

void ContactScanner::displayError(QAbstractSocket::SocketError error) {
	log.debug("displayError(%s)", Q(error));
}
