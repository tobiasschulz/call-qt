#include "contact.h"
#include "config.h"

const QString Contact::INVALID_USER("nobody");
const QHostAddress Contact::INVALID_HOST;
const quint16 Contact::INVALID_PORT = 0;
const Contact Contact::INVALID_CONTACT;

Contact::Contact(QString user, QHostAddress host, quint16 port, QObject* parent)
		: QObject(parent), m_user(user), m_host(host), m_port(port) {
}
Contact::Contact(QObject* parent)
		: QObject(parent), m_user(INVALID_USER), m_host(INVALID_HOST), m_port(INVALID_PORT) {
}
Contact::Contact(const Contact& other)
		: QObject(other.parent()) { //, m_user(other.m_user), m_host(other.m_host), m_port(other.m_port) {
	m_user = other.m_user;
	m_host = other.m_host;
	m_port = other.m_port;
}
Contact& Contact::operator=(const Contact& other) {
	m_user = QString(other.m_user);
	m_host = QHostAddress(other.m_host);
	m_port = other.m_port;
	return *this;
}
bool Contact::operator==(const Contact& other) const {
	return m_user == other.m_user && m_host == other.m_host && m_port == other.m_port;
}

QString Contact::getUser() const {
	return m_user;
}
QHostAddress Contact::getHost() const {
	return m_host;
}
quint16 Contact::getPort() const {
	return m_port;
}

QString Contact::toString() const {
	if (m_port == Config::DEFAULT_PORT)
		return m_user + "@" + m_host.toString();
	else
		return m_user + "@" + m_host.toString() + ":" + QString::number(m_port);
}
QString Contact::id() const {
	return "Contact<" + m_user + "@" + m_host.toString() + ":" + QString::number(m_port) + ">";
}

uint qHash(const Contact& c) {
	return qHash(c.id());
}
bool compareContacts(const Contact& left, const Contact& right) {
	return left.id() < right.id();
}

