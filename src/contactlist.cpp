#include "contactlist.h"

ContactList* ContactList::m_instance;
QMutex ContactList::m_mutex;

ContactList::ContactList(QObject *parent)
		: QObject(parent), m_set(), m_list() {
}

ContactList* ContactList::instance() {
	static QMutex mutex;
	if (!m_instance) {
		mutex.lock();
		if (!m_instance)
			m_instance = new ContactList;
		mutex.unlock();
	}
	return m_instance;
}

QString ContactList::id() const {
	return "ContactList";
}

void ContactList::addContact(Contact contact) {
	log.debug("add: %1", contact.id());
	m_set << contact;
	buildSortedList();
}
const Contact& ContactList::getContact(int index) const {
	return index < m_list.size() ? m_list.at(index) : Contact::INVALID_CONTACT;
}
int ContactList::size() const {
	int size = 0;
	size = m_list.size();
	return size;
}
void ContactList::buildSortedList() {
	emit this->beginInsertItems(0, m_set.size());
	m_mutex.lock();
	m_list = QList<Contact>::fromSet(m_set);
	qSort(m_list.begin(), m_list.end(), compareContacts);
	m_mutex.unlock();
	emit this->endInsertItems();
}
void ContactList::onResetContacts() {
	emit this->beginRemoveItems(0, m_set.size());
	m_set.clear();
	buildSortedList();
	emit this->endInsertItems();
}
