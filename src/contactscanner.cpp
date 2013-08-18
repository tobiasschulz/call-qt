#include "QtGlobal"

#include "contactscanner.h"
#include "statusconnection.h"
#include "contactlist.h"
#include "config.h"

ContactScanner::ContactScanner(QObject* parent, ContactList* contacts)
		: QThread(parent), m_contacts(contacts) {
}

void ContactScanner::run() {
	qDebug() << "hello from worker thread " << thread()->currentThreadId();

	foreach (const QString & host, Config::hosts_to_contact())
	{
		StatusConnection* connection;
		if (m_connections.contains(host)) {
			connection = m_connections[host];
		} else {
			connection = new StatusConnection(QHostAddress(host), Config::DEFAULT_PORT);
			QObject::connect(connection, &StatusConnection::readyRead, this, &ContactScanner::readyRead);
			m_connections[host] = connection;
		}
	}
}

void ContactScanner::readyRead() {
	qDebug() << "readyRead()";
}

void ContactScanner::displayError(QAbstractSocket::SocketError error) {
	qDebug() << "displayError()" << error;
}
