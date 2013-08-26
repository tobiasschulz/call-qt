#include <QtGlobal>
#include <QTimer>
#include <QSettings>
#include <QBuffer>

#include "contact.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "connection.h"
#include "pingclient.h"
#include "config.h"
#include "networkutil.h"

template<typename T>
QStringList serializeList(QList<T> list)
{
	QStringList list2;
	foreach (T obj, list)
	{
		list2.append(obj.id());
	}
	return list2;
}

template<typename T>
QList<T> deserializeList(QStringList list)
{
	QList<T> list2;
	foreach (QString str, list)
	{
		T obj;
		fromId(str, obj);
		list2.append(obj);
	}
	return list2;
}

ContactScanner::ContactScanner(QObject* parent)
		: QThread(parent), m_connections(), m_unknownhosts(), m_knownhosts(), m_hosts_mutex()
{
}

void ContactScanner::increasePriority(Host host)
{
	m_hosts_mutex.lock();
	if (m_unknownhosts.contains(host))
		m_unknownhosts.removeAll(host);
	if (m_knownhosts.contains(host))
		m_knownhosts.removeAll(host);
	m_knownhosts.prepend(host);
	m_hosts_mutex.unlock();

	QSettings settings;
	settings.setValue("contacts/known-hosts", serializeList(m_knownhosts));
}

QString ContactScanner::id() const
{
	return "ContactScanner";
}

void ContactScanner::run()
{
	m_unknownhosts << Config::hosts_to_contact();
	QObject::connect(ContactList::instance(), &ContactList::hostOnline, this,
			&ContactScanner::increasePriority);

	QSettings settings;
	m_knownhosts = deserializeList<Host>(
			settings.value("contacts/known-hosts", QStringList()).toStringList());
	foreach (const Host& host, m_knownhosts)
	{
		log.debug("known host: %1", Log::print(host));
		if (m_unknownhosts.contains(host))
			m_unknownhosts.removeAll(host);
	}

	QTimer *timer = new QTimer();
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(scanNow()));
	timer->start(Config::CONTACT_SCAN_INTERVAL);

	exec();
}

void ContactScanner::scanSoon()
{
	QTimer::singleShot(500, this, SLOT(scanNow()));
}

void ContactScanner::scanNow()
{
	log.debug("scan()");

	foreach (const Host & host, m_knownhosts)
	{
		if (!m_connections.contains(host)) {
			m_connections[host] = new PingClient(host, this);
		}
		emit m_connections[host]->ping();
		log.debug("pingclient: %1 (immediately)", Log::print(m_connections[host]));
	}

	int interval = Config::CONTACT_SCAN_INTERVAL / (1 + m_unknownhosts.size());
	int i = 0;
	foreach (const Host & host, m_unknownhosts)
	{
		if (!m_connections.contains(host)) {
			m_connections[host] = new PingClient(host, this);
		}
		int after = interval * (i / 10 + 1);
		QTimer::singleShot(after, m_connections[host], SLOT(ping()));
		log.debug("pingclient: %1 (after %2 ms)", Log::print(m_connections[host]), after);
		++i;
	}
}

void ContactScanner::onDisplayError(QAbstractSocket::SocketError error)
{
	log.debug("onDisplayError(%s)", Q(error));
}
