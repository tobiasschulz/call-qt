#include "contactmodel.h"
#include "contactlist.h"
#include "contactscanner.h"

ContactModel::ContactModel(QObject* parent)
		: QAbstractItemModel(parent), m_contactlist(0), m_scanner(0) {
	m_contactlist = new ContactList(this);
	QObject::connect(m_contactlist, &ContactList::beginInsertItems, this, &ContactModel::beginInsertItems);
	QObject::connect(m_contactlist, &ContactList::endInsertItems, this, &ContactModel::endInsertItems);
	QObject::connect(m_contactlist, &ContactList::beginRemoveItems, this, &ContactModel::beginRemoveItems);
	QObject::connect(m_contactlist, &ContactList::endRemoveItems, this, &ContactModel::endRemoveItems);
	QObject::connect(m_contactlist, &ContactList::changeItems, this, &ContactModel::changeItems);
	m_scanner = new ContactScanner(m_contactlist, this);
	QObject::connect(this, &ContactModel::resetContacts, m_scanner, &ContactScanner::onResetContacts);
	m_scanner->start();
}

int ContactModel::rowCount(const QModelIndex& parent) const {
	return m_contactlist && !parent.isValid() ? m_contactlist->size() : 0;
}

int ContactModel::columnCount(const QModelIndex& parent) const {
	return !parent.isValid() ? 1 : 0;
}

QModelIndex ContactModel::index(int row, int column, const QModelIndex& parent) const {
	return !parent.isValid() && row >= 0 && row < rowCount(parent) && column >= 0 && column < columnCount(parent) ?
			createIndex(row, column) : QModelIndex();
}

QModelIndex ContactModel::parent(const QModelIndex& child) const {
	Q_UNUSED(child);
	return QModelIndex();
}

QVariant ContactModel::data(const QModelIndex& index, int role) const {
	if (index.isValid() && role == Qt::DisplayRole) {
		if (m_contactlist && index.row() < m_contactlist->size()) {
			const Contact& contact = m_contactlist->getContact(index.row());
			QVariant value = contact.toString();
			return value;
		} else {
			return QVariant();
		}
	} else {
		return QVariant();
	}
}

bool ContactModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	Q_UNUSED(index);
	Q_UNUSED(value);
	Q_UNUSED(role);
	// m_data[index] = value;
	// emit dataChanged(index, index);
	// return true;
	return false;
}

void ContactModel::onResetContacts() {
	m_contactlist->onResetContacts();
	emit resetContacts();
}

void ContactModel::beginInsertItems(int start, int end) {
	// m_data.clear();
	beginInsertRows(QModelIndex(), start, end);
}

void ContactModel::endInsertItems() {
	endInsertRows();
}

void ContactModel::beginRemoveItems(int start, int end) {
	// m_data.clear();
	beginRemoveRows(QModelIndex(), start, end);
}

void ContactModel::endRemoveItems() {
	endInsertRows();
}

void ContactModel::changeItems(int start, int end) {
	// m_data.clear();
	emit dataChanged(index(start, 0), index(end, columnCount(QModelIndex())));
}
