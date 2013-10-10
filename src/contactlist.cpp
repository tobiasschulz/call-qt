#include "contactlist.h"

std::function<List::Contacts*(void)> ContactList = List::contacts;
std::function<List::Hosts*(void)> HostStates = List::hoststates;
std::function<List::UnknownHosts*(void)> UnknownHostList = List::unknownhosts;

List::Contacts* List::contacts()
{
	static QMutex lock;
	QMutexLocker locker(&lock);
	return &List::Contacts::instance();
}

List::Hosts* List::hoststates()
{
	static QMutex lock;
	QMutexLocker locker(&lock);
	return &List::Hosts::instance();
}

List::UnknownHosts* List::unknownhosts()
{
	static QMutex lock;
	QMutexLocker locker(&lock);
	return &List::UnknownHosts::instance();
}

void List::addSignals(Connection* connection)
{
	contacts()->addSignals(connection);
	hoststates()->addSignals(connection);
}
