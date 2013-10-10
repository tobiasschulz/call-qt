#include "model-unknownhosts.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "config.h"
#include "moviedelegate.h"

using namespace Model;

UnknownHosts::UnknownHosts(Abstract* parentmodel, QObject* parent)
		: Abstract(parentmodel, parent)
{
	List::UnknownHosts* m_contactlist = ContactList::unknownhosts();
	QObject::connect(m_contactlist, &List::UnknownHosts::beginListReset, this, &UnknownHosts::beginSetItems);
	QObject::connect(m_contactlist, &List::UnknownHosts::endListReset, this, &UnknownHosts::endSetItems);
	QObject::connect(m_contactlist, &List::UnknownHosts::itemChanged, this, &UnknownHosts::onStateChanged);
}

QString UnknownHosts::id() const
{
	return "Model::UnknownHosts";
}

int UnknownHosts::size() const
{
	return ContactList::unknownhosts()->size();
}

QVariant UnknownHosts::data(const QModelIndex& index, int role) const
{
	if (index.isValid() && index.row() < ContactList::unknownhosts()->size()) {
		QString hostname = ContactList::unknownhosts()->get(index.row());

		if (role == Qt::DisplayRole) {
			QVariant value = hostname;
			return value;
		} else if (role == Qt::DecorationRole) {
			List::Hosts::HostStateSet states = ContactList::hosts()->hostState(hostname);
			if (states.contains(List::Hosts::CONNECTING) || states.contains(List::Hosts::DNS_LOOKUP)) {
				return qVariantFromValue(Config::instance()->movie("reload", "gif"));
				//return Config::instance()->icon("reload", "gif");
			} else {
				return Config::instance()->icon("user-disabled");
			}
		} else if (role == Qt::TextColorRole) {
			return QColor(Qt::darkGray);
		} else {
			return QVariant();
		}
	} else {
		return QVariant();
	}
}

const Contact& UnknownHosts::getContact(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return Contact::INVALID_CONTACT;
}

