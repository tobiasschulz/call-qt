#include <QTimer>

#include "model-contacts.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "config.h"
#include "moviedelegate.h"
#include "maingui.h"

using namespace Model;

Contacts::Contacts(Abstract* parentmodel, QObject* parent)
		: Abstract(parentmodel, parent)
{
	List::Contacts* contactlist = ContactList();
	QObject::connect(contactlist, &List::Contacts::beginListReset, this, &Contacts::beginSetItems);
	QObject::connect(contactlist, &List::Contacts::endListReset, this, &Contacts::endSetItems);
	QObject::connect(contactlist, &List::Contacts::itemChanged, this, &Contacts::onStateChanged);

	Thread* thread = new Thread("Contacts", this);
	thread->start();
	ContactScanner* scanner = new ContactScanner();
	scanner->moveToThread(thread);
	QObject::connect(contactlist, &List::Contacts::resetContacts, scanner, &ContactScanner::scanSoon);
	QTimer::singleShot(0, scanner, SLOT(start()));

	Main::instance()->settingsContactList()->listen("show-hosts")->connect(this, SLOT(setVisible(bool)))->pushValue();
}

QString Contacts::id() const
{
	return "Model::Contacts";
}

int Contacts::internalSize() const
{
	return ContactList()->size();
}

QVariant Contacts::data(const QModelIndex& index, int role) const
{
	if (index.isValid() && index.row() < ContactList()->size()) {
		const Contact& contact = ContactList()->get(index.row());

		if (role == Qt::DisplayRole && index.column() == 1) {
			QVariant value = contact.user().username() + "@" + contact.address().toString();
			return value;
		} else if (role == Qt::DecorationRole && index.column() == 0) {
			List::Hosts::HostStateSet states = HostStates()->hostState(contact.host());
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

Contact Contacts::getContact(const QModelIndex& index) const
{
	if (index.isValid() && index.row() < ContactList()->size()) {
		const Contact& contact = ContactList()->get(index.row());
		return contact;
	} else {
		return Contact::INVALID_CONTACT;
	}
}

User Contacts::getUser(const QModelIndex& index) const
{
	return getContact(index).user();
}
