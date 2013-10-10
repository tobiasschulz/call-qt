#include <QTimer>

#include "model-contacts.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "config.h"
#include "moviedelegate.h"

using namespace Model;

Contacts::Contacts(Abstract* parentmodel, QObject* parent)
		: Abstract(parentmodel, parent)
{
	List::Contacts* contactlist = ContactList::contacts();
	QObject::connect(contactlist, &List::Contacts::beginListReset, this, &Contacts::beginSetItems);
	QObject::connect(contactlist, &List::Contacts::endListReset, this, &Contacts::endSetItems);
	QObject::connect(contactlist, &List::Contacts::itemChanged, this, &Contacts::onStateChanged);

	Thread* thread = new Thread("Contacts", this);
	thread->start();
	ContactScanner* scanner = new ContactScanner();
	scanner->moveToThread(thread);
	QObject::connect(contactlist, &List::Contacts::resetContacts, scanner, &ContactScanner::scanSoon);
	QTimer::singleShot(0, scanner, SLOT(start()));
}

QString Contacts::id() const
{
	return "Model::Contacts";
}

int Contacts::size() const
{
	return ContactList::contacts()->size();
}

QVariant Contacts::data(const QModelIndex& index, int role) const
{
	if (index.isValid() && index.row() < ContactList::contacts()->size()) {
		const Contact& contact = ContactList::contacts()->get(index.row());

		if (role == Qt::DisplayRole) {
			QVariant value = contact.toString();
			return value;
		} else if (role == Qt::DecorationRole) {
			List::Hosts::HostStateSet states = ContactList::hosts()->hostState(contact.host());
			if (states.contains(List::Hosts::CONNECTING) || states.contains(List::Hosts::DNS_LOOKUP)) {
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

const Contact& Contacts::getContact(const QModelIndex& index) const
{
	if (index.isValid() && index.row() < ContactList::contacts()->size()) {
		const Contact& contact = ContactList::contacts()->get(index.row());
		return contact;
	} else {
		return Contact::INVALID_CONTACT;
	}
}
