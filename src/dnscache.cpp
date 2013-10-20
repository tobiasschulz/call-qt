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

QHostInfo DnsCache::lookup(QString host, LookupMode mode, const Host* relatedHost)
{
	if (host.size() > 0) {
		if (m_hash.contains(host)) {
			return m_hash[host];
		} else if (mode == BLOCK_IF_NEEDED) {
			QMutexLocker locker(&m_mutex);
			if (relatedHost != 0)
				HostStates()->addHostState(*relatedHost, List::Hosts::DNS_LOOKUP);
			QHostInfo info = QHostInfo::fromName(host);
			if (relatedHost != 0)
				HostStates()->removeHostState(*relatedHost, List::Hosts::DNS_LOOKUP);
			QString addressStr = info.addresses().size() > 0 ? info.addresses().first().toString() : "";
			QString hostname = info.hostName();
			log.debug("blocking lookup: %1 = %2", addressStr, hostname);
			if (addressStr.size() > 0)
				m_hash[addressStr] = info;
			m_hash[hostname] = info;
			m_hash[host] = info;
			return m_hash[host];
		} else {
			return QHostInfo();
		}
	} else {
		return QHostInfo();
	}
}

QString DnsCache::lookup(QString host, HostInfo preferred, LookupMode mode, const Host* relatedHost)
{
	QHostInfo info = lookup(host, mode, relatedHost);

	if (preferred == HOSTNAME) {
		return info.hostName().size() > 0 ? info.hostName() : host;
	} else if (preferred == ADDRESS) {
		return info.addresses().size() > 0 ? info.addresses().at(0).toString() : host;
	} else {
		return host;
	}
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

QStringList DnsCache::lookup(QStringList hosts, HostInfo preferred, LookupMode mode)
{
	QHash<QString, QHostInfo> resolved = lookup(hosts, mode);

	QStringList mapped;
	QHash<QString, QHostInfo>::const_iterator i = resolved.constBegin();
	for (; i != resolved.constEnd(); ++i) {
		QHostInfo info = i.value();
		if (preferred == HOSTNAME) {
			mapped << (info.hostName().size() > 0 ? info.hostName() : i.key());
		} else if (preferred == ADDRESS) {
			mapped << (info.addresses().size() > 0 ? info.addresses().at(0).toString() : i.key());
		} else {
			mapped << i.key();
		}
	}
	mapped = mapped.toSet().toList();
	return mapped;
}

bool DnsCache::isCached(QString host)
{
	QMutexLocker locker(&m_mutex);
	return m_hash.contains(host);
}
