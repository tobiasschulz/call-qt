#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QObject>

#include "id.h"
#include "connection.h"

#include "list-contacts.h"
#include "list-hosts.h"
#include "list-unknownhosts.h"

class ContactList: public QObject, public ID
{
Q_OBJECT
public:
	static List::Contacts* contacts();
	static List::Hosts* hosts();
	static List::UnknownHosts* unknownhosts();
	QString id() const;

	static void addSignals(Connection* connection);

private:
	explicit ContactList(QObject* parent = 0);
};

#endif // CONTACTLIST_H
