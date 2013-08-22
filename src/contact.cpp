#include "contact.h"
#include "networkutil.h"
#include "config.h"

const QHostAddress Host::INVALID_ADDRESS;
const QString Host::INVALID_HOSTNAME;
const quint16 Host::INVALID_PORT = 0;

const Contact Contact::INVALID_CONTACT;
const QString Contact::INVALID_USER("nobody");
const Host Contact::INVALID_HOST;

Host::Host(QHostAddress address, quint16 port, QObject* parent)
		: QObject(parent), m_address(address), m_hostname(), m_port(port) {
	//m_hostname = NetworkUtil::parseAddress(m_address);
}
Host::Host(QObject* parent)
		: QObject(parent), m_address(), m_hostname(), m_port(0) {
}
Host::Host(const Host& other)
		: QObject(other.parent()), m_address(), m_hostname(), m_port(0) {
	m_address = QHostAddress(other.m_address);
	m_hostname = QString(other.m_hostname);
	m_port = other.m_port;
}

Host& Host::operator=(const Host& other) {
	m_address = QHostAddress(other.m_address);
	m_hostname = QString(other.m_hostname);
	m_port = other.m_port;
	return *this;
}
bool Host::operator==(const Host& other) const {
	return m_address == other.m_address && m_port == other.m_port;
}

QHostAddress Host::address() const {
	return m_address;
}
QString Host::hostname() const {
	return m_hostname.isEmpty() ? m_address.toString() : m_hostname;
}
quint16 Host::port() const {
	return m_port;
}

QString Host::toString() const {
	if (m_port == Config::DEFAULT_PORT)
		return hostname();
	else
		return hostname() + ":" + QString::number(m_port);
}
QString Host::id() const {
	return "Host<" + m_address.toString() + ":" + QString::number(m_port) + ">";
}

Contact::Contact(QString user, QHostAddress host, quint16 port, QObject* parent)
		: QObject(parent), m_user(user), m_host(host, port) {
}
Contact::Contact(QString user, Host host, QObject* parent)
		: QObject(parent), m_user(user), m_host(host) {
}
Contact::Contact(QObject* parent)
		: QObject(parent), m_user(), m_host() {
}
Contact::Contact(const Contact& other)
		: QObject(other.parent()) { //, m_user(other.m_user), m_host(other.m_host), m_port(other.m_port) {
	m_user = other.m_user;
	m_host = other.m_host;
}

Contact& Contact::operator=(const Contact& other) {
	m_user = QString(other.m_user);
	m_host = Host(other.m_host);
	return *this;
}
bool Contact::operator==(const Contact& other) const {
	return m_user == other.m_user && m_host == other.m_host;
}

QString Contact::user() const {
	return m_user;
}
Host Contact::host() const {
	return m_host;
}
QHostAddress Contact::address() const {
	return m_host.address();
}
QString Contact::hostname() const {
	return m_host.hostname();
}
quint16 Contact::port() const {
	return m_host.port();
}

QString Contact::toString() const {
	return m_user + "@" + m_host.toString();
	if (m_host.port() == Config::DEFAULT_PORT)
		return m_user + "@" + hostname();
	else
		return m_user + "@" + hostname() + ":" + QString::number(m_host.port());
}
QString Contact::id() const {
	return "Contact<" + m_user + "@" + m_host.id() + ">";
	return "Contact<" + m_user + "@" + m_host.toString() + ":" + QString::number(m_host.port()) + ">";
}

bool compareContacts(const Contact& left, const Contact& right) {
	return left.id() < right.id();
}

QDataStream& operator<<(QDataStream& out, const Host& myObj) {
	out << QString("Host") << myObj.address() << myObj.hostname() << quint32(myObj.port());
	return out;
}
QDataStream& operator>>(QDataStream& in, Host& myObj) {
	QHostAddress address;
	QString hostname;
	quint32 port;
	QString type;
	in >> type;
	if (type == "Host") {
		in >> address >> hostname >> port;
		if (!hostname.isEmpty()) {
			QHostAddress currentAddress = NetworkUtil::instance()->parseHostname(hostname);
			myObj = Host(currentAddress, port);
		} else {
			myObj = Host(address, port);
		}
	} else {
		qDebug() << "Error in deserialization of type Host: invalid type '" + type + "'!";
		myObj = Contact::INVALID_HOST;
	}
	return in;
}

QDataStream& operator<<(QDataStream& out, const Contact& myObj) {
	out << QString("Contact") << myObj.user() << myObj.host();
	return out;
}
QDataStream& operator>>(QDataStream& in, Contact& myObj) {
	QString user;
	Host host;
	QString type;
	in >> type;
	if (type == "Contact") {
		in >> user >> host;
		myObj = Contact(user, host);
	} else {
		qDebug() << "Error in deserialization of type Contact: invalid type '" + type + "'!";
		myObj = Contact::INVALID_CONTACT;
	}
	return in;
}
