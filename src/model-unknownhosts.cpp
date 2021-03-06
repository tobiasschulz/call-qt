#include <QAction>

#include "model-unknownhosts.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "config.h"
#include "moviedelegate.h"
#include "maingui.h"

using namespace Model;

UnknownHosts::UnknownHosts(Abstract* parentmodel, QObject* parent)
		: Abstract(parentmodel, parent)
{
	List::UnknownHosts* unknownhosts = UnknownHostList();
	QObject::connect(unknownhosts, &List::UnknownHosts::beginListReset, this, &UnknownHosts::beginSetItems);
	QObject::connect(unknownhosts, &List::UnknownHosts::endListReset, this, &UnknownHosts::endSetItems);
	QObject::connect(unknownhosts, &List::UnknownHosts::itemChanged, this, &UnknownHosts::onStateChanged);

	Main::instance()->settingsContactList()->listen("show-offline-hosts")->connect(this, SLOT(setVisible(bool)))->pushValue();
}

QString UnknownHosts::id() const
{
	return "Model::UnknownHosts";
}

int UnknownHosts::internalSize() const
{
	return UnknownHostList()->size();
}

QVariant UnknownHosts::data(const QModelIndex& index, int role) const
{
	if (index.isValid() && index.row() < UnknownHostList()->size()) {
		QString hostname = UnknownHostList()->get(index.row());

		if (role == Qt::TextColorRole) {
			return QColor(Qt::darkGray);
		} else if (role == Qt::DisplayRole && index.column() == 1) {
			QVariant value = hostname;
			return value;
		} else if (role == Qt::DecorationRole && index.column() == 0) {
			List::Hosts::HostStateSet states = HostStates()->hostState(hostname);
			if (m_showConnections
					&& (states.contains(List::Hosts::CONNECTING) || states.contains(List::Hosts::DNS_LOOKUP))) {
				return qVariantFromValue(Config::instance()->movie("reload", "gif"));
				//return Config::instance()->icon("reload", "gif");
			} else {
				return Config::instance()->icon("user-disabled");
			}
		} else {
			return QVariant();
		}
	} else {
		return QVariant();
	}
}

Contact UnknownHosts::getContact(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return Contact::INVALID_CONTACT;
}

User UnknownHosts::getUser(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return User::INVALID_USER;
}

