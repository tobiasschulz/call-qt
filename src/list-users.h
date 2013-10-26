/*
 * list-users.h
 *
 *  Created on: 25.10.2013
 *      Author: tobias
 */

#ifndef LIST_USERS_H_
#define LIST_USERS_H_

#include <QObject>
#include <QList>
#include <QMutex>
#include <QMutexLocker>

#include "contact.h"
#include "connection.h"
#include "list-abstract.h"

class ContactList;

namespace List
{
	class Users: public Abstract
	{
	Q_OBJECT
	public:
		static Users& instance();
		QString id() const;

		const User& get(int index) const;
		int size() const;
		void addSignals(Connection* connection);
		QList<User> toList() const;

	public slots:
		void add(User user);
		void addContact(Contact contact);
		void onHostStateChanged(Host host);

	private:
		explicit Users(QObject* parent = 0);

		void rebuildItems();

		QList<User> m_users;
		static QMutex m_lock;

		friend class ContactList;
	};
}

#endif /* LIST_USERS_H_ */
