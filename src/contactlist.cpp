#include "contactlist.h"

List::Contacts* ContactList::contacts()
{
	static QMutex lock;
	QMutexLocker locker(&lock);
	return &List::Contacts::instance();
}

List::Hosts* ContactList::hosts()
{
	static QMutex lock;
	QMutexLocker locker(&lock);
	return &List::Hosts::instance();
}

List::UnknownHosts* ContactList::unknownhosts()
{
	static QMutex lock;
	QMutexLocker locker(&lock);
	return &List::UnknownHosts::instance();
}

QString ContactList::id() const
{
	return "ContactList";
}

void ContactList::addSignals(Connection* connection) {
	contacts()->addSignals(connection);
	hosts()->addSignals(connection);
}
