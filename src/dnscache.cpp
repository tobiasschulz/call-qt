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
		mutex.unlock();
	}
	return m_instance;
}
DnsCache::DnsCache(QObject* parent)
		: QThread(parent), m_hash(), m_mutex()
{
}
DnsCache::~DnsCache()
{
}
QString DnsCache::id() const
{
	return "DnsCache";
}
void DnsCache::run()
{
	exec();
}
void DnsCache::lookup(QString host)
{
	QMutexLocker locker(&m_mutex);
	if (m_hash.contains(host)) {
		emit lookedUp(m_hash[host]);
	} else {
		QHostInfo::lookupHost(host, this, SLOT(onLookedUp(QHostInfo)));
	}
}
QHostInfo DnsCache::forceLookup(QString host)
{
	QMutexLocker locker(&m_mutex);
	if (m_hash.contains(host)) {
		return m_hash[host];
	} else {
		m_hash[host] = QHostInfo::fromName(host);
		return m_hash[host];
	}
}
void DnsCache::onLookedUp(QHostInfo info)
{
	QMutexLocker locker(&m_mutex);
	QString addressStr = info.addresses().size() > 0 ? info.addresses().first().toString() : "";
	QString hostname = info.hostName();
	log.debug("onLookedUp(%1, %2)", addressStr, hostname);
	m_hash[addressStr] = info;
	m_hash[hostname] = info;
	emit lookedUp(info);
}
