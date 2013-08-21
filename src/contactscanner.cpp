#include "QtGlobal"
#include "QTimer"

#include "contactscanner.h"
#include "statusconnection.h"
#include "contactlist.h"
#include "config.h"
#include "networkutil.h"

ContactScanner::ContactScanner(QObject* parent)
		: QThread(parent) {
}

QString ContactScanner::id() const {
	return "ContactScanner";
}

void ContactScanner::run() {
	QTimer *timer = new QTimer();
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(scanNow()));
	timer->start(30000);

	exec();
}

void ContactScanner::scanSoon() {
	QTimer::singleShot(500, this, SLOT(scanNow()));
}

void ContactScanner::scanNow() {
	log.debug("scan()");

	foreach (const QString & hostname, Config::hosts_to_contact())
	{
		StatusConnection* connection;
		if (m_connections.contains(hostname)) {
			connection = m_connections[hostname];

			if (!connection->isConnected()) {
				delete connection;
				connection = connect(hostname, Config::DEFAULT_PORT);
				m_connections[hostname] = connection;
			}

		} else {
			connection = connect(hostname, Config::DEFAULT_PORT);
			m_connections[hostname] = connection;
		}

		log.debug("connection: %1, state: %2", connection->id(), connection->isConnected() ? "true" : "false");
	}
}

StatusConnection* ContactScanner::connect(QString hostname, quint16 port) {
	QHostAddress hostaddr = NetworkUtil::parseHostname(hostname);
	StatusConnection* connection = new StatusConnection(hostaddr, Config::DEFAULT_PORT);
	QObject::connect(connection, &StatusConnection::contactFound, ContactList::instance(), &ContactList::addContact);
	return connection;
}

void ContactScanner::onDisplayError(QAbstractSocket::SocketError error) {
	log.debug("onDisplayError(%s)", Q(error));
}
