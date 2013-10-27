/*
 * list-contacts.cpp
 *
 *  Created on: 10.10.2013
 *      Author: tobias
 */

#include <QSettings>

#include "list-contacts.h"
#include "list-hosts.h"
#include "config.h"
#include "dnscache.h"

using namespace List;

QMutex Contacts::m_lock;

Contacts::Contacts(QObject *parent)
		: Abstract(parent), m_contacts()
{
	QObject::connect(&Hosts::instance(), &Hosts::hostStateChanged, this, &Contacts::onHostStateChanged);
}

Contacts& Contacts::instance()
{
	static Contacts instance;
	return instance;
}

QString Contacts::id() const
{
	return "List::Contacts";
}

void Contacts::add(Contact contact)
{
	if (contact != Contact::INVALID_CONTACT && !m_contacts.contains(contact)) {
		log.debug("add: %1", contact.id());
		m_contacts << contact;
		rebuildItems();
	}
}

const Contact& Contacts::get(int index) const
{
	QMutexLocker locker(&m_lock);
	return index < m_contacts.size() ? m_contacts.at(index) : Contact::INVALID_CONTACT;
}

QList<Contact> Contacts::toList() const
{
	return m_contacts;
}

const Contact& Contacts::___reachableContact(const Contact& unreachable) const
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

int Contacts::size() const
{
	QMutexLocker locker(&m_lock);
	return m_contacts.size();
}

void Contacts::rebuildItems()
{
	// create contact unique list from set
	QList<Contact> contacts(m_contacts);
	qSort(contacts.begin(), contacts.end(), compareContacts);

	emit beginListReset(m_contacts.size(), contacts.size());
	{
		QMutexLocker locker(&m_lock);
		m_contacts = contacts;
	}
	emit endListReset();
}

void Contacts::onResetContacts()
{
	emit this->beginListReset(m_contacts.size(), 0);
	emit this->endListReset();
	m_contacts.clear();
	rebuildItems();
	emit resetContacts();
}

void Contacts::addSignals(Connection* connection)
{
	QObject::connect(connection, &Connection::contactFound, this, &Contacts::add);
}

void Contacts::onHostStateChanged(Host host)
{
	QList<int> changedContacts;
	{
		QMutexLocker locker(&m_lock);
		for (int i = 0; i < m_contacts.size(); ++i) {
			if (host == m_contacts[i].host()) {
				changedContacts << i;
			}
		}
	}
	foreach (int i, changedContacts)
	{
		emit itemChanged(i);
	}
}

