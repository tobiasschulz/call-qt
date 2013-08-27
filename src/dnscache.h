#ifndef DNSCACHE_H
#define DNSCACHE_H

#include <QThread>
#include <QHostInfo>
#include <QMutex>

#include "id.h"
#include "thread.h"

class DnsCache: public QObject, public ID
{
Q_OBJECT
public:
	static DnsCache* instance();
	virtual ~DnsCache();
	QString id() const;

	QHostInfo forceLookup(QString host);
	bool isCached(QString host);
	QHostInfo cachedLookup(QString host);

signals:
	void lookedUp(QHostInfo info);

public slots:
	void lookup(QString hostname);
	void onLookedUp(QHostInfo info);

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
