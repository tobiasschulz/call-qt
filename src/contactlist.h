#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QObject>
#include "contact.h"

class ContactList: public QObject, public Id {
Q_OBJECT
public:
	explicit ContactList(QObject* parent = 0);
	QString id() const;

	int size() const;
	const Contact& getContact(int index) const;

signals:

public slots:

private:
	QList<Contact> m_list;
};

#endif // CONTACTLIST_H
