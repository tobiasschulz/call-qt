#include <QSettings>

#include "contactlist.h"
#include "config.h"
#include "dnscache.h"

ContactList* ContactList::m_instance;
QMutex ContactList::m_lock;

ContactList::ContactList(QObject *parent)
		: QObject(parent), m_contacts(), m_hoststate(), m_unknownhosts()
{
	QObject::connect(this, &ContactList::hostChanged, this, &ContactList::onHostStateChanged);
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
	if (contact != Contact::INVALID_CONTACT && !m_contacts.contains(contact)) {
		log.debug("add: %1", contact.id());
		m_contacts << contact;
		rebuildItems();
	}
}

const Contact& ContactList::contact(int index) const
{
	QMutexLocker locker(&m_lock);
	return index < m_contacts.size() ? m_contacts.at(index) : Contact::INVALID_CONTACT;
}

const Contact& ContactList::reachableContact(const Contact& unreachable) const
{
	if (unreachable.host().isUnreachable() && !unreachable.host().isLoopback()) {
		foreach (const Contact& contact, m_contacts)
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
	return m_contacts.size();
}

void ContactList::rebuildItems()
{
	// create contact unique list from set
	QList<Contact> contacts(m_contacts);
	qSort(contacts.begin(), contacts.end(), compareContacts);

	emit this->beginSetContacts(m_contacts.size(), contacts.size());
	{
		QMutexLocker locker(&m_lock);
		m_contacts = contacts;
	}
	emit this->endSetContacts();

	// create unknown/offline host list
	QStringList unknownhosts;
	QSettings settings;
	unknownhosts << Config::instance()->hostnames(Config::KNOWN_HOST);
	unknownhosts << Config::instance()->defaultHostnames();
	foreach (const Contact& contact, contacts)
	{
		unknownhosts.removeAll(contact.hostname());
		unknownhosts.removeAll(contact.address().toString());
	}
	unknownhosts = DnsCache::instance()->lookup(unknownhosts, DnsCache::HOSTNAME, DnsCache::CACHE_ONLY);
	qSort(unknownhosts.begin(), unknownhosts.end(), compareHostnamesAndAddresses);

	foreach (QString host, unknownhosts)
	{
		log.debug("host = %1", host);
	}

	emit this->beginSetUnknownHosts(m_unknownhosts.size(), unknownhosts.size());
	{
		QMutexLocker locker(&m_lock);
		m_unknownhosts = unknownhosts;
	}
	emit this->endSetUnknownHosts();
}

QStringList ContactList::unknownHosts()
{
	return m_unknownhosts;
}

void ContactList::onResetContacts()
{
	emit this->beginSetContacts(m_contacts.size(), 0);
	emit this->endSetContacts();
	m_contacts.clear();
	rebuildItems();
	emit resetContacts();
}

void ContactList::setHostOnline(Host host)
{
	if (!m_hoststate[host].contains(HOST_ONLINE)) {
		m_hoststate[host] << HOST_ONLINE;
		emit hostOnline(host);
	}
}

void ContactList::setHostOffline(Host host)
{
	if (m_hoststate[host].contains(HOST_ONLINE)) {
		m_hoststate[host].remove(HOST_ONLINE);
		emit hostOffline(host);
	}
}

bool ContactList::isHostOnline(Host host)
{
	return m_hoststate[host].contains(HOST_ONLINE);
}

void ContactList::addSignals(Connection* connection)
{
	QObject::connect(connection, &Connection::contactFound, this, &ContactList::addContact);
	QObject::connect(connection, &Connection::hostOnline, this, &ContactList::setHostOnline);
	QObject::connect(connection, &Connection::hostOffline, this, &ContactList::setHostOffline);
}

void ContactList::addHostState(Host host, HostState state)
{
	m_hoststate[host] << state;
	emit hostChanged(host);
}

void ContactList::removeHostState(Host host, HostState state)
{
	m_hoststate[host].remove(state);
	emit hostChanged(host);
}

ContactList::HostStateSet ContactList::hostState(Host host)
{
	return m_hoststate[host];
}

ContactList::HostStateSet ContactList::hostState(QString hostname)
{
	HostStateSet state;
	foreach (const Host& host, m_hoststate.keys())
	{
		if (hostname == host.hostname() || hostname == host.address().toString()) {
			state.unite(m_hoststate[host]);
		}
	}
	return state;
}

void ContactList::onHostStateChanged(Host host)
{
	QList<int> changedContacts;
	QList<int> changedUnknownHosts;
	{
		QMutexLocker locker(&m_lock);
		for (int i = 0; i < m_contacts.size(); ++i) {
			if (host == m_contacts[i].host()) {
				changedContacts << i;
			}
		}
		for (int i = 0; i < m_unknownhosts.size(); ++i) {
			if (host.hostname() == m_unknownhosts[i] || host.address().toString() == m_unknownhosts[i]) {
				changedUnknownHosts << i;
			}
		}
	}
	foreach (int i, changedContacts)
	{
		emit contactStateChanged(i);
	}
	foreach (int i, changedUnknownHosts)
	{
		emit unknownHostStateChanged(i);
	}
}

