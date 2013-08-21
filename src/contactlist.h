#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QObject>
#include <QList>
#include <QSet>
#include <QMutex>

#include "contact.h"

class ContactList: public QObject, public Id {
Q_OBJECT
public:
	static ContactList* instance();
	QString id() const;

	const Contact& getContact(int index) const;
	int size() const;

signals:
	void beginInsertItems(int start, int end);
	void endInsertItems();
	void beginRemoveItems(int start, int end);
	void endRemoveItems();
	void changeItems(int start, int end);

public slots:
	void addContact(Contact contact);
	void onResetContacts();

private:
	explicit ContactList(QObject* parent = 0);

	void buildSortedList();

	static ContactList* m_instance;
	QSet<Contact> m_set;
	QList<Contact> m_list;
	static QMutex m_mutex;
};

#endif // CONTACTLIST_H
