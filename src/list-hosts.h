/*
 * list-hosts.h
 *
 *  Created on: 10.10.2013
 *      Author: tobias
 */

#ifndef LIST_HOSTS_H_
#define LIST_HOSTS_H_

#include <QObject>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>

#include "contact.h"
#include "connection.h"
#include "list-abstract.h"

namespace List
{
	class Hosts: public QObject, public ID
	{
	Q_OBJECT
	public:
		static Hosts& instance();
		QString id() const;
		void addSignals(Connection* connection);

		enum HostState
		{
			CONNECTING, DNS_LOOKUP, HOST_ONLINE
		};
		typedef QSet<HostState> HostStateSet;

		bool isHostOnline(Host host);
		HostStateSet hostState(Host host);
		HostStateSet hostState(QString hostname);
		HostStateSet hostState(QList<Host> hosts);

	signals:
		void hostOnline(Host host);
		void hostOffline(Host host);
		void hostStateChanged(Host host);

	public slots:
		void setHostOnline(Host host);
		void setHostOffline(Host host);
		void addHostState(Host host, HostState state);
		void removeHostState(Host host, HostState state);

	private:
		explicit Hosts(QObject* parent = 0);

		void rebuildItems();

		QHash<Host, HostStateSet> m_hoststate;
		static QMutex m_lock;

		friend class ContactList;
	};
}

#endif /* LIST_HOSTS_H_ */
