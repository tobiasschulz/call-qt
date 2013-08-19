#include "contact.h"

const Contact Contact::INVALID_CONTACT(QString(), QHostAddress(), 0);

Contact::Contact(QString user, QHostAddress host, quint16 port, QObject* parent)
		: QObject(parent), m_user(user), m_host(host), m_port(port) {
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
	return m_user + "@" + m_host.toString() + ":" + m_port;
}
QString Contact::id() const {
	return "Contact<" + m_user + "@" + m_host.toString() + ":" + m_port + ">";
}
