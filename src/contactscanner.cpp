#include "QtGlobal"
#include "QTimer"

#include "contactscanner.h"
#include "connection.h"
#include "pingclient.h"
#include "contactlist.h"
#include "config.h"
#include "networkutil.h"

ContactScanner::ContactScanner(QObject* parent)
		: QThread(parent), m_connections(), m_hosts(), m_hosts_mutex() {
	m_hosts << Config::hosts_to_contact();
	QObject::connect(ContactList::instance(), &ContactList::hostOnline, this, &ContactScanner::increasePriority);
}

void ContactScanner::increasePriority(Host host) {
	m_hosts_mutex.lock();
	if (m_hosts.contains(host))
		m_hosts.removeAll(host);
	m_hosts.prepend(host);
	m_hosts_mutex.unlock();
}

QString ContactScanner::id() const {
	return "ContactScanner";
}

void ContactScanner::run() {
	QTimer *timer = new QTimer();
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(scanNow()));
	timer->start(Config::CONTACT_SCAN_INTERVAL);

	exec();
}

void ContactScanner::scanSoon() {
	QTimer::singleShot(500, this, SLOT(scanNow()));
}

void ContactScanner::scanNow() {
	log.debug("scan()");

	int interval = Config::CONTACT_SCAN_INTERVAL / m_hosts.size();
	int i = 0;
	foreach (const Host & host, m_hosts)
	{
		if (!m_connections.contains(host)) {
			m_connections[host] = new PingClient(host, this);
		}
		int after = interval * (i / 10);
		QTimer::singleShot(after, m_connections[host], SLOT(ping()));
		log.debug("pingclient: %1 (after %2 ms)", Log::print(m_connections[host]), after);
	}
}

void ContactScanner::onDisplayError(QAbstractSocket::SocketError error) {
	log.debug("onDisplayError(%s)", Q(error));
}
