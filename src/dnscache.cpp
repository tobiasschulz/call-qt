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
		: QObject(parent), m_hash(), m_mutex()
{
}
DnsCache::~DnsCache()
{
}
QString DnsCache::id() const
{
	return "DnsCache";
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
	if (host.size() > 0) {
		QMutexLocker locker(&m_mutex);
		if (m_hash.contains(host)) {
			return m_hash[host];
		} else {
			QHostInfo info = QHostInfo::fromName(host);
			QString addressStr = info.addresses().size() > 0 ? info.addresses().first().toString() : "";
			QString hostname = info.hostName();
			log.debug("blocking lookup: %1 = %2", addressStr, hostname);
			if (addressStr.size() > 0)
				m_hash[addressStr] = info;
			m_hash[hostname] = info;
			m_hash[host] = info;
			return m_hash[host];
		}
	} else {
		return QHostInfo();
	}
}
bool DnsCache::isCached(QString host)
{
	QMutexLocker locker(&m_mutex);
	return m_hash.contains(host);
}
void DnsCache::onLookedUp(QHostInfo info)
{
	QMutexLocker locker(&m_mutex);
	QString addressStr = info.addresses().size() > 0 ? info.addresses().first().toString() : "";
	QString hostname = info.hostName();
	log.debug("delayed lookup: %1 = %2", addressStr, hostname);
	if (addressStr.size() > 0)
		m_hash[addressStr] = info;
	m_hash[hostname] = info;
	emit lookedUp(info);
}
