#ifndef DNSCACHE_H
#define DNSCACHE_H

#include <QThread>
#include <QHostInfo>
#include <QHostAddress>
#include <QMutex>
#include <QList>
#include <QHash>
#include <QString>
#include <QStringList>

#include "id.h"
#include "thread.h"
#include "contact.h"
#include "contactlist.h"

class DnsCache: public QObject, public ID
{
Q_OBJECT
public:
	static DnsCache* instance();
	virtual ~DnsCache();
	QString id() const;

	enum LookupMode
	{
		CACHE_ONLY, BLOCK_IF_NEEDED
	};
	enum HostInfo
	{
		HOSTNAME, ADDRESS
	};

	QHostInfo lookup(QString host, LookupMode mode = BLOCK_IF_NEEDED, const Host* relatedHost = 0);
	QString lookup(QString host, HostInfo preferred, LookupMode mode = BLOCK_IF_NEEDED, const Host* relatedHost = 0);
	QHash<QString, QHostInfo> lookup(QStringList hosts, LookupMode mode = BLOCK_IF_NEEDED);
	QStringList lookup(QStringList hosts, HostInfo preferred, LookupMode mode = BLOCK_IF_NEEDED);
	bool isCached(QString host);

private:
	explicit DnsCache(QObject* parent = 0);
	DnsCache(const DnsCache &); // hide copy constructor
	DnsCache& operator=(const DnsCache &); // hide assign op
	// we leave just the declarations, so the compiler will warn us
	// if we try to use those two functions by accident
	static DnsCache* m_instance;

	QHash<QString, QHostInfo> m_hash;
	QMutex m_mutex;
};

#endif // DNSCACHE_H
