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
	ContactList* contactlist = ContactList::instance();
	QObject::connect(contactlist, &ContactList::beginSetContacts, this, &Contacts::beginSetItems);
	QObject::connect(contactlist, &ContactList::endSetContacts, this, &Contacts::endSetItems);
	QObject::connect(contactlist, &ContactList::contactStateChanged, this, &Contacts::onStateChanged);

	Thread* thread = new Thread("Contacts", this);
	thread->start();
	ContactScanner* scanner = new ContactScanner();
	scanner->moveToThread(thread);
	QObject::connect(contactlist, &ContactList::resetContacts, scanner, &ContactScanner::scanSoon);
	QTimer::singleShot(0, scanner, SLOT(start()));
}

QString Contacts::id() const
{
	return "Model::Contacts";
}

int Contacts::size() const
{
	return ContactList::instance()->size();
}

QVariant Contacts::data(const QModelIndex& index, int role) const
{
	QPointer<ContactList> contactlist = ContactList::instance();
	int indexContact = index.row();

	if (index.isValid() && indexContact >= 0 && indexContact < contactlist->size()) {
		const Contact& contact = contactlist->contact(index.row());

		if (role == Qt::DisplayRole) {
			QVariant value = contact.toString();
			return value;
		} else if (role == Qt::DecorationRole) {
			ContactList::HostStateSet states = contactlist->hostState(contact.host());
			if (states.contains(ContactList::CONNECTING) || states.contains(ContactList::DNS_LOOKUP)) {
				return qVariantFromValue(Config::instance()->movie("reload", "gif"));
			} else if (states.contains(ContactList::HOST_ONLINE)) {
				return Config::instance()->icon("user-available");
			} else {
				return Config::instance()->icon("user-offline");
			}
		} else {
			return QVariant();
		}
	}

	QStringList unknownhosts = contactlist->unknownHosts();
	int indexHost = index.row() - contactlist->size();

	if (index.isValid() && indexHost >= 0 && indexHost < unknownhosts.size()) {
		QString hostname = unknownhosts.at(indexHost);

		if (role == Qt::DisplayRole) {
			QVariant value = hostname;
			return value;
		} else if (role == Qt::DecorationRole) {
			ContactList::HostStateSet states = contactlist->hostState(hostname);
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

const Contact& Contacts::getContact(const QModelIndex& index) const
{
	if (index.isValid() && index.row() < ContactList::instance()->size()) {
		const Contact& contact = ContactList::instance()->contact(index.row());
		return contact;
	} else {
		return Contact::INVALID_CONTACT;
	}
}
