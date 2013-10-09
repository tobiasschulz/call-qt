#include "model-unknownhosts.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "config.h"
#include "moviedelegate.h"

using namespace Model;

UnknownHosts::UnknownHosts(Abstract* parentmodel, QObject* parent)
		: Abstract(parentmodel, parent)
{
	ContactList* m_contactlist = ContactList::instance();
	QObject::connect(m_contactlist, &ContactList::beginSetUnknownHosts, this, &UnknownHosts::beginSetItems);
	QObject::connect(m_contactlist, &ContactList::endSetUnknownHosts, this, &UnknownHosts::endSetItems);
	QObject::connect(m_contactlist, &ContactList::unknownHostStateChanged, this, &UnknownHosts::onStateChanged);
}

QString UnknownHosts::id() const
{
	return "Model::UnknownHosts";
}

int UnknownHosts::size() const
{
	return ContactList::instance()->unknownHosts().size();
}

QVariant UnknownHosts::data(const QModelIndex& index, int role) const
{
	QStringList unknownhosts = ContactList::instance()->unknownHosts();

	if (index.isValid() && index.row() < unknownhosts.size()) {
		QString hostname = unknownhosts.at(index.row());

		if (role == Qt::DisplayRole) {
			QVariant value = hostname;
			return value;
		} else if (role == Qt::DecorationRole) {
			ContactList::HostStateSet states = ContactList::instance()->hostState(hostname);
			if (states.contains(ContactList::CONNECTING) || states.contains(ContactList::DNS_LOOKUP)) {
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

