#include "contactlist.h"

ContactList::ContactList(QObject *parent)
		: QObject(parent) {
}

int ContactList::size() const {
	return m_list.size();
}
const Contact& ContactList::getContact(int index) const {
	return index < m_list.size() ? m_list.at(index) : Contact::INVALID_CONTACT;
}
