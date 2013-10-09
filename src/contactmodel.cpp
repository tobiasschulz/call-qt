#include <QTimer>

#include "contactmodel.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "config.h"
#include "moviedelegate.h"

ContactModel::ContactModel(QObject* parent)
		: QAbstractItemModel(parent)
{

	ContactList* m_contactlist = ContactList::instance();
	QObject::connect(m_contactlist, &ContactList::beginSetContacts, this, &ContactModel::onBeginSetContacts);
	QObject::connect(m_contactlist, &ContactList::endSetContacts, this, &ContactModel::onEndSetContacts);
	QObject::connect(m_contactlist, &ContactList::beginSetUnknownHosts, this, &ContactModel::onBeginSetUnknownHosts);
	QObject::connect(m_contactlist, &ContactList::endSetUnknownHosts, this, &ContactModel::onEndSetUnknownHosts);
	QObject::connect(m_contactlist, &ContactList::contactStateChanged, this, &ContactModel::onContactStateChanged);
	QObject::connect(m_contactlist, &ContactList::unknownHostStateChanged, this,
			&ContactModel::onUnknownHostStateChanged);

	Thread* thread = new Thread("Contacts", this);
	thread->start();
	ContactScanner* scanner = new ContactScanner();
	scanner->moveToThread(thread);
	QObject::connect(this, &ContactModel::resetContacts, m_contactlist, &ContactList::onResetContacts);
	QObject::connect(this, &ContactModel::resetContacts, scanner, &ContactScanner::scanSoon);
	QTimer::singleShot(0, scanner, SLOT(start()));
}

QString ContactModel::id() const
{
	return "ContactModel";
}

int ContactModel::rowCount(const QModelIndex& parent) const
{
	return !parent.isValid() ? ContactList::instance()->size() + ContactList::instance()->unknownHosts().size() : 0;
}

int ContactModel::columnCount(const QModelIndex& parent) const
{
	return !parent.isValid() ? 1 : 0;
}

QModelIndex ContactModel::index(int row, int column, const QModelIndex& parent) const
{
	return !parent.isValid() && row >= 0 && row < rowCount(parent) && column >= 0 && column < columnCount(parent) ?
			createIndex(row, column) : QModelIndex();
}

QModelIndex ContactModel::parent(const QModelIndex& child) const
{
	Q_UNUSED(child);
	return QModelIndex();
}

QVariant ContactModel::data(const QModelIndex& index, int role) const
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

const Contact& ContactModel::getContact(const QModelIndex& index) const
{
	if (index.isValid() && index.row() < ContactList::instance()->size()) {
		const Contact& contact = ContactList::instance()->contact(index.row());
		return contact;
	} else {
		return Contact::INVALID_CONTACT;
	}
}

bool ContactModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	Q_UNUSED(index);
	Q_UNUSED(value);
	Q_UNUSED(role);
// m_data[index] = value;
// emit dataChanged(index, index);
// return true;
	return false;
}

void ContactModel::onResetContacts()
{
	emit resetContacts();
}

void ContactModel::beginInsertItems(int start, int end)
{
	beginInsertRows(QModelIndex(), start, end);
}

void ContactModel::endInsertItems()
{
	endInsertRows();
}

void ContactModel::beginRemoveItems(int start, int end)
{
	beginRemoveRows(QModelIndex(), start, end);
}

void ContactModel::endRemoveItems()
{
	endRemoveRows();
}

void ContactModel::changeItems(int start, int end)
{
	emit dataChanged(index(start, 0), index(end, columnCount(QModelIndex())));
}

void ContactModel::onContactStateChanged(int i)
{
	log.debug("onContactStateChanged: %1", i);
	changeItems(i, i);
}

void ContactModel::onUnknownHostStateChanged(int i)
{
	int contacts = ContactList::instance()->size();
	changeItems(contacts + i, contacts + i);
}

void ContactModel::onBeginSetContacts(int oldcount, int newcount)
{
	beginRemoveItems(0, oldcount);
	endRemoveItems();
	beginInsertItems(0, newcount);
}

void ContactModel::onEndSetContacts()
{
	endInsertItems();
}

void ContactModel::onBeginSetUnknownHosts(int oldcount, int newcount)
{
	int countContacts = ContactList::instance()->size();
	beginRemoveItems(countContacts, oldcount);
	endRemoveItems();
	beginInsertItems(countContacts, newcount);
}

void ContactModel::onEndSetUnknownHosts()
{
	endInsertItems();
}

