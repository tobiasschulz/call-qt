#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QObject>
#include <functional>

#include "id.h"
#include "connection.h"

#include "list-contacts.h"
#include "list-hosts.h"
#include "list-unknownhosts.h"

namespace List
{
	List::Contacts* contacts();
	List::Hosts* hoststates();
	List::UnknownHosts* unknownhosts();
	void addSignals(Connection* connection);
}

extern std::function<List::Contacts*(void)> ContactList;
extern std::function<List::Hosts*(void)> HostStates;
extern std::function<List::UnknownHosts*(void)> UnknownHostList;
//constexpr auto ContactList = List::contacts;
//constexpr auto HostStates = List::hoststates;
//constexpr auto UnknownHostList = List::unknownhosts;

#endif // CONTACTLIST_H
