#include <QtGlobal>
#include <QTimer>
#include <QBuffer>

#include "id.h"
#include "contact.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "connection.h"
#include "pingclient.h"
#include "config.h"
#include "networkutil.h"

ContactScanner::ContactScanner(QObject* parent)
		: QObject(parent), m_connections()
{
}

void ContactScanner::increasePriority(Host host)
{
	if (host == Host::INVALID_HOST) {
		// invalid
	} else if (host.isUnreachable() && 0) {
		log.debug("host is unreachable: %1", Log::print(host));
		QList<quint16> ports = Config::instance()->defaultPorts();
		QList<Host> hosts;
		foreach (quint16 port, ports)
		{
			host = Host(host.address(), port);
			if (!m_connections.contains(host)) {
				m_connections[host] = new PingClient(host, this);
			}
			emit m_connections[host]->ping();
			log.debug("=> pingclient: %1 (immediately)", Log::print(m_connections[host]));
		}
	} else {
		Config::instance()->addHost(host, Config::KNOWN_HOST);
	}
}

QString ContactScanner::id() const
{
	return "ContactScanner";
}

void ContactScanner::start()
{
	QObject::connect(ContactList::instance(), &ContactList::hostOnline, this, &ContactScanner::increasePriority);

	QTimer *timer = new QTimer();
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(scanNow()));
	timer->start(Config::instance()->CONTACT_SCAN_INTERVAL);
}

void ContactScanner::scanSoon()
{
	QTimer::singleShot(500, this, SLOT(scanNow()));
}

void ContactScanner::scanNow()
{
	log.debug("scan()");
	QList<Host> unknownhosts = Config::instance()->hosts(Config::UNKNOWN_HOST);
	QList<Host> knownhosts = Config::instance()->hosts(Config::KNOWN_HOST);

	foreach (const Host & host, knownhosts)
	{
		if (!m_connections.contains(host)) {
			m_connections[host] = new PingClient(host, this);
		}
//		emit m_connections[host]->ping();
		QTimer::singleShot(100, m_connections[host], SLOT(ping()));
		log.debug("pingclient: %1 (immediately)", Log::print(m_connections[host]));
	}

	int interval = Config::instance()->CONTACT_SCAN_INTERVAL / (1 + unknownhosts.size() / 5);
	int i = 0;
	foreach (const Host & host, unknownhosts)
	{
		if (!m_connections.contains(host)) {
			m_connections[host] = new PingClient(host, this);
		}
		int after = interval * (i / 5 + 1);
		QTimer::singleShot(after, m_connections[host], SLOT(ping()));
		log.debug("pingclient: %1 (after %2 ms)", Log::print(m_connections[host]), after);
		++i;
	}
}

void ContactScanner::onDisplayError(QAbstractSocket::SocketError error)
{
	log.error("onDisplayError(%s)", qPrintable(error));
}
