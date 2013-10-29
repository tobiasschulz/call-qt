/*
 * list-contacts.h
 *
 *  Created on: 10.10.2013
 *      Author: tobias
 */

#ifndef LIST_CONTACTS_H_
#define LIST_CONTACTS_H_

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
	class Contacts: public Abstract
	{
	Q_OBJECT
	public:
		static Contacts& instance();
		QString id() const;

		const Contact& get(int index) const;
		int size() const;
		void addSignals(Connection* connection);
		QList<Contact> toList() const;

	signals:
		void resetContacts();

	public slots:
		void add(Contact contact);
		void onResetContacts();
		void onHostStateChanged(Host host);

	private:
		explicit Contacts(QObject* parent = 0);

		void rebuildItems();

		QList<Contact> m_contacts;
		static QMutex m_lock;

		friend class ContactList;
	};
}

#endif /* LIST_CONTACTS_H_ */
