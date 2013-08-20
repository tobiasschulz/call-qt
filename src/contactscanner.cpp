#include "QtGlobal"
#include "QTimer"

#include "contactscanner.h"
#include "statusconnection.h"
#include "contactlist.h"
#include "config.h"
#include "networkutil.h"

ContactScanner::ContactScanner(ContactList* contactlist, QObject* parent)
		: QThread(parent), m_contactlist(contactlist) {
}
QString ContactScanner::id() const {
	return "ContactScanner";
}

void ContactScanner::run() {
	QTimer *timer = new QTimer();
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(scan()));
	timer->start(30000);
	scan();

	exec();
}

void ContactScanner::scan() {
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
	QHostAddress hostaddr = NetworkUtil::parseHost(hostname);
	StatusConnection* connection = new StatusConnection(hostaddr, Config::DEFAULT_PORT);
	QObject::connect(connection, &StatusConnection::readyRead, this, &ContactScanner::onReadyRead);
	QObject::connect(connection, &StatusConnection::contactFound, this, &ContactScanner::onContactFound);
	return connection;
}

void ContactScanner::onContactFound(Contact* contact) {
	log.debug("onContactFound(%1)", contact->toString());
	m_contactlist->addContact(*contact);
}
void ContactScanner::onResetContacts() {
	scan();
}

void ContactScanner::onReadyRead() {
	log.debug("onReadyRead()");
}

void ContactScanner::onDisplayError(QAbstractSocket::SocketError error) {
	log.debug("onDisplayError(%s)", Q(error));
}
