/*
 * list-unknownhosts.cpp
 *
 *  Created on: 10.10.2013
 *      Author: tobias
 */

#include <QSettings>

#include "list-unknownhosts.h"
#include "list-contacts.h"
#include "list-hosts.h"
#include "config.h"
#include "dnscache.h"

using namespace List;

QMutex UnknownHosts::m_lock;

UnknownHosts::UnknownHosts(QObject *parent)
		: Abstract(parent), m_unknownhosts()
{
	QObject::connect(&Hosts::instance(), &Hosts::hostStateChanged, this, &UnknownHosts::onHostStateChanged);
	QObject::connect(&Contacts::instance(), &Contacts::resetContacts, this, &UnknownHosts::rebuildItems);
}

UnknownHosts& UnknownHosts::instance()
{
	static UnknownHosts instance;
	return instance;
}

QString UnknownHosts::id() const
{
	return "List::UnknownHosts";
}

const QString& UnknownHosts::get(int index) const
{
	QMutexLocker locker(&m_lock);
	return index < m_unknownhosts.size() ? m_unknownhosts.at(index) : "";
}

int UnknownHosts::size() const
{
	QMutexLocker locker(&m_lock);
	return m_unknownhosts.size();
}

void UnknownHosts::rebuildItems()
{
	// create unknown/offline host list
	QStringList unknownhosts;
	QSettings settings;
	unknownhosts << Config::instance()->hostnames(Config::KNOWN_HOST);
	unknownhosts << Config::instance()->defaultHostnames();

	QList<Contact> contacts(Contacts::instance().toList());
	foreach (const Contact& contact, contacts)
	{
		unknownhosts.removeAll(contact.displayname());
	}

	unknownhosts = DnsCache::instance()->lookup(unknownhosts, DnsCache::HOSTNAME, DnsCache::CACHE_ONLY);
	qSort(unknownhosts.begin(), unknownhosts.end(), compareHostnamesAndAddresses);

	foreach (QString host, unknownhosts)
	{
		log.debug("host = %1", host);
	}

	emit beginListReset(m_unknownhosts.size(), unknownhosts.size());
	{
		QMutexLocker locker(&m_lock);
		m_unknownhosts = unknownhosts;
	}
	emit endListReset();
}

QList<QString> UnknownHosts::toList() const
{
	return m_unknownhosts;
}

void UnknownHosts::onHostStateChanged(Host host)
{
	log.debug("onHostStateChanged: %1", Log::print(host));
	QList<int> changedUnknownHosts;
	{
		QMutexLocker locker(&m_lock);
		for (int i = 0; i < m_unknownhosts.size(); ++i) {
			if (host.displayname() == m_unknownhosts[i]) {
				changedUnknownHosts << i;
			}
		}
	}
	foreach (int i, changedUnknownHosts)
	{
		emit itemChanged(i);
	}
}

