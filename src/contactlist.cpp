#include <QSettings>

#include "contactlist.h"
#include "config.h"
#include "dnscache.h"

ContactList* ContactList::m_instance;
QMutex ContactList::m_lock;

ContactList::ContactList(QObject *parent)
		: QObject(parent), m_set(), m_onlinehosts(), m_list(), m_unknownhosts()
{
}

ContactList* ContactList::instance()
{
	static QMutex mutex;
	if (!m_instance) {
		mutex.lock();
		if (!m_instance)
			m_instance = new ContactList;
		mutex.unlock();
	}
	return m_instance;
}

QString ContactList::id() const
{
	return "ContactList";
}

void ContactList::addContact(Contact contact)
{
	if (contact != Contact::INVALID_CONTACT) {
		log.debug("add: %1", contact.id());
		m_set << contact;
		buildSortedList();
	}
}

const Contact& ContactList::getContact(int index) const
{
	QMutexLocker locker(&m_lock);
	return index < m_list.size() ? m_list.at(index) : Contact::INVALID_CONTACT;
}

const Contact& ContactList::getReachableContact(const Contact& unreachable) const
{
	if (unreachable.host().isUnreachable() && !unreachable.host().isLoopback()) {
		foreach (const Contact& contact, m_set)
		{
			if (contact.user() == unreachable.user() && contact.host().address() == unreachable.host().address()
					&& contact.host().isReachable()) {
				return contact;
			}
		}
	}
	return unreachable;
}

int ContactList::size() const
{
	QMutexLocker locker(&m_lock);
	return m_list.size();
}

void ContactList::buildSortedList()
{
	emit this->beginRemoveItems(0, m_list.size());
	emit this->endRemoveItems();

	// create contact unique list from set
	QList<Contact> list;
	list = QList<Contact>::fromSet(m_set);
	qSort(list.begin(), list.end(), compareContacts);

	// create unknown/offline host list
	QStringList offlinehosts;
	QSettings settings;
	offlinehosts << Config::instance()->hostnames(Config::KNOWN_HOST);
	offlinehosts << Config::instance()->defaultHostnames();
	foreach (const Contact& contact, list)
	{
		offlinehosts.removeAll(contact.hostname());
		offlinehosts.removeAll(contact.address().toString());
	}
	offlinehosts = DnsCache::instance()->lookup(offlinehosts, DnsCache::HOSTNAME, DnsCache::CACHE_ONLY);
	qSort(offlinehosts.begin(), offlinehosts.end(), compareHostnamesAndAddresses);

	emit this->beginInsertItems(0, list.size());
	{
		QMutexLocker locker(&m_lock);
		m_list = list;
		m_unknownhosts = offlinehosts;
		foreach (QString host, offlinehosts)
		{
			log.debug("host = %1", host);
		}
	}
	emit this->endInsertItems();
}

QStringList ContactList::unknownHosts()
{
	return m_unknownhosts;
}

void ContactList::onResetContacts()
{
	emit this->beginRemoveItems(0, m_set.size());
	m_set.clear();
	buildSortedList();
	emit this->endRemoveItems();
}

void ContactList::setHostOnline(Host host)
{
	if (!m_onlinehosts.contains(host)) {
		m_onlinehosts << host;
		emit hostOnline(host);
	}
}

void ContactList::setHostOffline(Host host)
{
	if (m_onlinehosts.contains(host)) {
		m_onlinehosts.remove(host);
		emit hostOffline(host);
	}
}

bool ContactList::isHostOnline(Host host)
{
	return m_onlinehosts.contains(host);
}

void ContactList::addSignals(Connection* connection)
{
	QObject::connect(connection, &Connection::contactFound, this, &ContactList::addContact);
	QObject::connect(connection, &Connection::hostOnline, this, &ContactList::setHostOnline);
	QObject::connect(connection, &Connection::hostOffline, this, &ContactList::setHostOffline);
}

