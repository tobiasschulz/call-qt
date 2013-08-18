#include "contactmodel.h"
#include "contactlist.h"
#include "contactscanner.h"

ContactModel::ContactModel(QObject* parent)
		: QAbstractItemModel(parent), m_contacts(0) {
	m_contacts = new ContactList(this);
	m_scanner = new ContactScanner(this, m_contacts);
	m_scanner->start();
}

int ContactModel::rowCount(const QModelIndex& parent) const {
	return m_contacts && !parent.isValid() ? m_contacts->size() : 0;
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
		if (m_contacts && index.row() < m_contacts->size()) {
			const Contact& contact = m_contacts->getContact(index.row());
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
