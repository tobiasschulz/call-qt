/*
 * list-users.cpp
 *
 *  Created on: 25.10.2013
 *      Author: tobias
 */

#include <QSettings>

#include "list-users.h"
#include "list-contacts.h"
#include "list-hosts.h"
#include "config.h"
#include "dnscache.h"

using namespace List;

QMutex Users::m_lock;

Users::Users(QObject *parent)
		: Abstract(parent), m_users()
{
	QObject::connect(&Hosts::instance(), &Hosts::hostStateChanged, this, &Users::onHostStateChanged);
	QObject::connect(&Contacts::instance(), &Contacts::resetContacts, this, &Users::onResetContacts);
}

Users& Users::instance()
{
	static Users instance;
	return instance;
}

QString Users::id() const
{
	return "List::Users";
}

void Users::add(User user)
{
	if (user != User::INVALID_USER && !m_users.contains(user)) {
		log.debug("add: %1", user.id());
				m_users << user;
		rebuildItems();
	}
}

void Users::addContact(Contact contact)
{
	log.debug("add: %1", contact.id());
	if (contact != Contact::INVALID_CONTACT && !m_users.contains(contact.user())) {
		m_users << contact.user();
		rebuildItems();
	}
	foreach (const User& user, m_users)
	{
		log.debug("add: users: - %1", user.toString());
	}
}

const User& Users::get(int index) const
{
	QMutexLocker locker(&m_lock);
	return index < m_users.size() ? m_users.at(index) : User::INVALID_USER;
}

QList<User> Users::toList() const
{
	return m_users;
}

int Users::size() const
{
	QMutexLocker locker(&m_lock);
	return m_users.size();
}

void Users::rebuildItems()
{
	// create contact unique list from set
	QList<User> users(m_users);
	qSort(users.begin(), users.end(), compareUsers);

	emit beginListReset(m_users.size(), users.size());
	{
		QMutexLocker locker(&m_lock);
		m_users = users;
	}
	emit endListReset();
}

void Users::onResetContacts()
{
	emit this->beginListReset(m_users.size(), 0);
	emit this->endListReset();
	m_users.clear();
	rebuildItems();
}

void Users::addSignals(Connection* connection)
{
	QObject::connect(connection, &Connection::contactFound, this, &Users::addContact);
	QObject::connect(connection, &Connection::userFound, this, &Users::add);
}

void Users::onHostStateChanged(Host host)
{
	QList<int> changedUsers;
	{
		QMutexLocker locker(&m_lock);
		QList<Contact> contacts = Contacts::instance().toList();
		foreach (const Contact& contact, contacts)
		{
			if (host == contact.host()) {
				for (int i = 0; i < m_users.size(); ++i) {
					if (m_users[i] == contact.user()) {
						changedUsers << i;
					}
				}
			}
		}
	}
	foreach (int i, changedUsers)
	{
		emit itemChanged(i);
	}
}

