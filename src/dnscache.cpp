#include <QtGlobal>
#include <QTimer>
#include <QSettings>

#include "dnscache.h"
#include "config.h"
#include "networkutil.h"

DnsCache* DnsCache::m_instance;

DnsCache* DnsCache::instance()
{
	static QMutex mutex;
	if (!m_instance) {
		mutex.lock();
		if (!m_instance)
			m_instance = new DnsCache;
		Thread* thread = new Thread("DnsCache");
		thread->start();
		m_instance->moveToThread(thread);
		mutex.unlock();
	}
	return m_instance;
}

DnsCache::DnsCache(QObject* parent)
		: QObject(parent), m_hash(), m_pendingLookups(), m_mutex()
{
	QObject::connect(this, &DnsCache::delayedLookup, this, &DnsCache::onDelayedLookup);
}

DnsCache::~DnsCache()
{
}

QString DnsCache::id() const
{
	return "DnsCache";
}

QHostInfo DnsCache::lookup(QString host, LookupMode mode)
{
	if (host.isEmpty()) {
		// invalid hostname
		return QHostInfo();

	} else if (m_hash.contains(host)) {
		// in cache
		return m_hash[host];

	} else if (mode == BLOCKING_LOOKUP) {
		// blocking lookup
		QMutexLocker locker(&m_mutex);
		QHostInfo info = QHostInfo::fromName(host);
		QString addressStr = info.addresses().size() > 0 ? info.addresses().first().toString() : "";
		QString hostname = info.hostName();
		log.debug("blocking lookup: %1 = %2", addressStr, hostname);
		if (addressStr.size() > 0)
			m_hash[addressStr] = info;
		m_hash[hostname] = info;
		m_hash[host] = info;
		return m_hash[host];

	} else if (m_pendingLookups.contains(host)) {
		// pending lookup
		return QHostInfo();

	} else {
		// delayed lookup
		m_pendingLookups << host;
		emit delayedLookup(host);
		return QHostInfo();
	}
}

void DnsCache::onDelayedLookup(QString host)
{
	QHostInfo info = QHostInfo::fromName(host);
	QString addressStr = info.addresses().size() > 0 ? info.addresses().first().toString() : "";
	QString hostname = info.hostName();
	log.debug("asynchronous lookup: %1 = %2", addressStr, hostname);
	if (addressStr.size() > 0)
		m_hash[addressStr] = info;
	m_hash[hostname] = info;
	m_hash[host] = info;
	m_pendingLookups.remove(host);
}

QHash<QString, QHostInfo> DnsCache::lookup(QStringList hosts, LookupMode mode)
{
	QHash<QString, QHostInfo> resolved;
	foreach (QString host, hosts)
	{
		resolved[host] = lookup(host, mode);
	}
	return resolved;
}

bool DnsCache::isCached(QString host)
{
	QMutexLocker locker(&m_mutex);
	return m_hash.contains(host);
}
