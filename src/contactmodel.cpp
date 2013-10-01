#include <QTimer>

#include "contactmodel.h"
#include "contactlist.h"
#include "contactscanner.h"
#include "config.h"

ContactModel::ContactModel(QObject* parent)
		: QAbstractItemModel(parent)
{

	ContactList* m_contactlist = ContactList::instance();
	QObject::connect(m_contactlist, &ContactList::beginInsertItems, this, &ContactModel::beginInsertItems);
	QObject::connect(m_contactlist, &ContactList::endInsertItems, this, &ContactModel::endInsertItems);
	QObject::connect(m_contactlist, &ContactList::beginRemoveItems, this, &ContactModel::beginRemoveItems);
	QObject::connect(m_contactlist, &ContactList::endRemoveItems, this, &ContactModel::endRemoveItems);
	QObject::connect(m_contactlist, &ContactList::changeItems, this, &ContactModel::changeItems);

	Thread* thread = new Thread("Contacts", this);
	thread->start();
	ContactScanner* scanner = new ContactScanner();
	scanner->moveToThread(thread);
	QObject::connect(this, &ContactModel::resetContacts, m_contactlist, &ContactList::onResetContacts);
	QObject::connect(this, &ContactModel::resetContacts, scanner, &ContactScanner::scanSoon);
	QTimer::singleShot(0, scanner, SLOT(start()));
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
	QStringList unknownhosts = contactlist->unknownHosts();
	int indexContact = index.row();
	int indexHost = index.row() - contactlist->size();

	if (index.isValid() && indexContact >= 0 && indexContact < contactlist->size()) {
		const Contact& contact = contactlist->getContact(index.row());

		if (role == Qt::DisplayRole) {
			QVariant value = contact.toString();
			return value;
		} else if (role == Qt::DecorationRole) {
			if (contactlist->isHostOnline(contact.host())) {
				return Config::instance()->icon("user-available");
			} else {
				return Config::instance()->icon("user-offline");
			}
		} else {
			return QVariant();
		}
	} else if (index.isValid() && indexHost >= 0 && indexHost < unknownhosts.size()) {
		QString hostname = unknownhosts.at(indexHost);

		if (role == Qt::DisplayRole) {
			QVariant value = hostname;
			return value;
		} else if (role == Qt::DecorationRole) {
			return Config::instance()->icon("user-disabled");
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
	if (index.isValid()) {
		if (index.row() < ContactList::instance()->size()) {
			const Contact& contact = ContactList::instance()->getContact(index.row());
			return contact;
		} else {
			return Contact::INVALID_CONTACT;
		}
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
// m_data.clear();
	beginInsertRows(QModelIndex(), start, end);
}

void ContactModel::endInsertItems()
{
	endInsertRows();
}

void ContactModel::beginRemoveItems(int start, int end)
{
// m_data.clear();
	beginRemoveRows(QModelIndex(), start, end);
}

void ContactModel::endRemoveItems()
{
	endInsertRows();
}

void ContactModel::changeItems(int start, int end)
{
// m_data.clear();
	emit dataChanged(index(start, 0), index(end, columnCount(QModelIndex())));
}
