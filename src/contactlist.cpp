#include "contactlist.h"

ContactList* ContactList::m_instance;
QMutex ContactList::m_lock;

ContactList::ContactList(QObject *parent)
		: QObject(parent), m_set(), m_onlinehosts(), m_list()
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
	log.debug("add: %1", contact.id());
	m_set << contact;
	buildSortedList();
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
	int size = 0;
	size = m_list.size();
	return size;
}

void ContactList::buildSortedList()
{
	emit this->beginRemoveItems(0, m_list.size());
	emit this->endRemoveItems();
	QList<Contact> list;
	list = QList<Contact>::fromSet(m_set);
	qSort(list.begin(), list.end(), compareContacts);
	emit this->beginInsertItems(0, list.size());
	{
		QMutexLocker locker(&m_lock);
		m_list = list;
	}
	emit this->endInsertItems();
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

