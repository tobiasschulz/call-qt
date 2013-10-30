#include <QTimer>

#include "model-users.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "config.h"
#include "moviedelegate.h"
#include "maingui.h"

using namespace Model;

Users::Users(Abstract* parentmodel, QObject* parent)
		: Abstract(parentmodel, parent), m_usernameFormat(FORMAT_FIRST)
{
	List::Users* userlist = UserList();
	QObject::connect(userlist, &List::Users::beginListReset, this, &Users::beginSetItems);
	QObject::connect(userlist, &List::Users::endListReset, this, &Users::endSetItems);
	QObject::connect(userlist, &List::Users::itemChanged, this, &Users::onStateChanged);

	Main::instance()->settingsContactList()->listen("user-name-format")->connect(this,
	SLOT(setUsernameFormat(QString)))->pushValue();
}

QString Users::id() const
{
	return "Model::Users";
}

int Users::internalSize() const
{
	return UserList()->size();
}

QVariant Users::data(const QModelIndex& index, int role) const
{
	if (index.isValid() && index.row() < UserList()->size()) {
		const User& user = UserList()->get(index.row());

		if (role == Qt::DisplayRole && index.column() == 1) {
			return formatUser(user);
		} else if (role == Qt::DecorationRole && index.column() == 0) {
			List::Hosts::HostStateSet states = HostStates()->hostState(user.hosts());
			if (m_showConnections
					&& (states.contains(List::Hosts::CONNECTING) || states.contains(List::Hosts::DNS_LOOKUP))) {
				return qVariantFromValue(Config::instance()->movie("reload", "gif"));
			} else if (states.contains(List::Hosts::HOST_ONLINE)) {
				return Config::instance()->icon("user-available");
			} else {
				return Config::instance()->icon("user-offline");
			}
		} else {
			return QVariant();
		}
	} else {
		return QVariant();
	}
}

Contact Users::getContact(const QModelIndex& index) const
{
	User user(getUser(index));
	QList<Host> hosts = user.hosts();
	return hosts.size() > 0 ? Contact(user, hosts[0]) : Contact::INVALID_CONTACT;
}

User Users::getUser(const QModelIndex& index) const
{
	if (index.isValid() && index.row() < UserList()->size()) {
		return UserList()->get(index.row());
	} else {
		return User::INVALID_USER;
	}
}

void Users::setUsernameFormat(QString value)
{
	m_usernameFormat = value == "system" ? FORMAT_SYSTEM : value == "first" ? FORMAT_FIRST : FORMAT_FULL;
	log.debug("setUsernameFormat(%1) = %2", value, QString::number(m_usernameFormat));
	refresh();
}

QString Users::formatUserName(const User& user) const
{
	switch (m_usernameFormat) {
	case FORMAT_SYSTEM:
		return user.username();
	case FORMAT_FIRST:
		return user.firstname();
	case FORMAT_FULL:
		return user.fullname();
	}
}

UsersWithComputername::UsersWithComputername(Abstract* parentmodel, QObject* parent)
		: Users(parentmodel, parent)
{
	Main::instance()->settingsContactList()->listen("show-users-computernames")->connect(this, SLOT(setVisible(bool)))->pushValue();
}

QString UsersWithComputername::formatUser(const User& user) const
{
	return formatUserName(user) + "@" + user.computername();
}

UsersWithoutComputername::UsersWithoutComputername(Abstract* parentmodel, QObject* parent)
		: Users(parentmodel, parent)
{
	Main::instance()->settingsContactList()->listen("show-users")->connect(this, SLOT(setVisible(bool)))->pushValue();
}

QString UsersWithoutComputername::formatUser(const User& user) const
{
	return formatUserName(user);
}

