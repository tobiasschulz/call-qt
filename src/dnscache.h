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

class DnsCache: public QObject, public ID
{
Q_OBJECT
public:
	static DnsCache* instance();
	virtual ~DnsCache();
	QString id() const;

	enum LookupMode
	{
		CACHE_LOOKUP, BLOCKING_LOOKUP
	};

	QHostInfo lookup(QString host, LookupMode mode = BLOCKING_LOOKUP);
	QHash<QString, QHostInfo> lookup(QStringList hosts, LookupMode mode = BLOCKING_LOOKUP);
	bool isCached(QString host);

signals:
	void delayedLookup(QString host);

public slots:
	void onDelayedLookup(QString host);

private:
	explicit DnsCache(QObject* parent = 0);
	DnsCache(const DnsCache &); // hide copy constructor
	DnsCache& operator=(const DnsCache &); // hide assign op
	// we leave just the declarations, so the compiler will warn us
	// if we try to use those two functions by accident
	static DnsCache* m_instance;

	QHash<QString, QHostInfo> m_hash;
	QSet<QString> m_pendingLookups;
	QMutex m_mutex;
};

#endif // DNSCACHE_H
