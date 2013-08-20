#include "contactlist.h"

ContactList::ContactList(QObject *parent)
		: QObject(parent), m_set(), m_list() {
}
QString ContactList::id() const {
	return "ContactList";
}

void ContactList::addContact(Contact contact) {
	m_set << contact;
	buildSortedList();
}
const Contact& ContactList::getContact(int index) const {
	return index < m_list.size() ? m_list.at(index) : Contact::INVALID_CONTACT;
}
int ContactList::size() const {
	return m_list.size();
}
void ContactList::buildSortedList() {
	emit this->beginInsertItems(0, m_set.size());
	m_list = QList<Contact>::fromSet(m_set);
	qSort(m_list.begin(), m_list.end(), compareContacts);
	emit this->endInsertItems();
}
void ContactList::onResetContacts() {
	m_list.clear();
	buildSortedList();
}
