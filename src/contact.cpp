#include <QThread>
#include <QTimer>

#include "contact.h"
#include "networkutil.h"
#include "dnscache.h"
#include "config.h"

const Host Host::INVALID_HOST;
const QHostAddress Host::INVALID_ADDRESS;
const QString Host::INVALID_HOSTNAME;
const quint16 Host::INVALID_PORT = 0;

const Contact Contact::INVALID_CONTACT;
const QString Contact::INVALID_USER("nobody");

Host::Host(QHostAddress address, quint16 port, QObject* parent)
		: QObject(parent), m_address(address), m_address_valid(true), m_hostname(), m_hostname_valid(false),
			m_port(port)
{
	lookupHostname();
}
Host::Host(QString hostname, quint16 port, QObject* parent)
		: QObject(parent), m_address(), m_address_valid(false), m_hostname(), m_hostname_valid(false), m_port(port)
{
	QHostAddress address(hostname);
	if (address.protocol() == QAbstractSocket::IPv4Protocol || address.protocol() == QAbstractSocket::IPv6Protocol) {
		m_address = address;
		m_address_valid = true;
		//log.debug("Valid IPv4 address: %1", m_address.toString());
		lookupHostname();
	} else {
		m_hostname = hostname;
		m_hostname_valid = true;
		//log.debug("Got a hostname: %1", hostname);
		lookupAddress();
	}
}
Host::Host(QObject * parent)
		: QObject(parent), m_address(), m_address_valid(true), m_hostname(), m_hostname_valid(true), m_port(0)
{
}
Host::Host(const Host& other)
		: QObject(other.parent()), m_address(), m_address_valid(), m_hostname(), m_hostname_valid(), m_port(0)
{
	m_address = QHostAddress(other.m_address);
	m_hostname = QString(other.m_hostname);
	m_port = other.m_port;
	m_address_valid = other.m_address_valid;
	if (!other.m_address_valid) {
		lookupAddress();
	}
	m_hostname_valid = other.m_hostname_valid;
	if (!other.m_hostname_valid) {
		lookupHostname();
	}
}

Host& Host::operator=(const Host& other)
{
	m_address = QHostAddress(other.m_address);
	m_hostname = QString(other.m_hostname);
	m_port = other.m_port;
	m_address_valid = other.m_address_valid;
	if (!other.m_address_valid) {
		lookupAddress();
	}
	m_hostname_valid = other.m_hostname_valid;
	if (!other.m_hostname_valid) {
		lookupHostname();
	}
	return *this;
}
bool Host::operator==(const Host& other) const
{
	return ((m_address_valid && m_address == other.m_address) || (m_hostname_valid && m_hostname == other.m_hostname))
			&& m_port == other.m_port;
}
bool Host::operator!=(const Host& other) const
{
	return !(*this == other);
}

QHostAddress Host::address() const
{
	return m_address;
}
QString Host::hostname() const
{
	if (!m_hostname_valid) {
		return m_address.toString();
	}
	return m_hostname.isEmpty() ? m_address.toString() : m_hostname;
}
quint16 Host::port() const
{
	return m_port;
}
void Host::lookupAddress()
{
	QHostInfo info = DnsCache::instance()->forceLookup(m_hostname);
	if (info.addresses().size() != 0) {
		m_address = info.addresses().first();
		m_address_valid = true;
		//log.debug("lookupAddress(%1) = %2", m_hostname, m_address.toString());
	} else {
		m_address = INVALID_ADDRESS;
		m_address_valid = false;
		log.debug("lookupAddress(%1) failed! no address found!", m_hostname);
	}
}
void Host::lookupHostname()
{
	QHostInfo info = DnsCache::instance()->forceLookup(m_address.toString());
	if (info.hostName().size() > 0) {
		m_hostname = info.hostName();
		m_hostname_valid = true;
		//log.debug("lookupAddress(%1) = %2", m_address.toString(), m_hostname);
	} else {
		m_hostname = INVALID_HOSTNAME;
		m_hostname_valid = false;
		log.debug("lookupAddress(%1) failed! no hostname found!", m_address.toString());
	}
}

QString Host::toString(PortFormat showPort, HostFormat hostFormat) const
{
	QString formattedHost = hostFormat == SHOW_HOSTNAME ? hostname() : address().toString();
	if (m_port == Config::DEFAULT_PORT && showPort == SHOW_PORT_ONLY_UNUSUAL)
		return formattedHost;
	else
		return formattedHost + ":" + QString::number(m_port);
}
QString Host::id() const
{
	return "Host<" + address().toString() + ":" + QString::number(m_port) + ">";
}
QString Host::print() const
{
	return "Host " + (m_address.toString().size() > 0 ? m_address.toString() : m_hostname) + ":"
			+ QString::number(m_port) + "";
}
QString Host::serialize() const
{
	return "Host<" + address().toString() + ":" + QString::number(m_port) + ">";
}
Host Host::deserialize(QString _str)
{
	StaticID id("Host::fromId");
	if (_str.startsWith("Host<") && _str.endsWith(">")) {
		QString str = _str.mid(5);
		str = str.left(str.size() - 1);
		QStringList parts = str.split(":");
		if (parts.size() == 2) {
			Host obj = Host(QHostAddress(parts[0]), parts[1].toInt());
			id.logger().debug("deserialization successful: %1 = %2", _str, Log::print(obj));
			return obj;
		}
	}
	id.logger().debug("deserialization failed: %1", _str);
	return Host::INVALID_HOST;
}

Contact::Contact(QString user, QHostAddress host, quint16 port, QObject* parent)
		: QObject(parent), m_user(user), m_host(host, port)
{
}
Contact::Contact(QString user, Host host, QObject* parent)
		: QObject(parent), m_user(user), m_host(host)
{
}
Contact::Contact(QObject * parent)
		: QObject(parent), m_user(), m_host()
{
}
Contact::Contact(const Contact& other)
		: QObject(other.parent())
{ //, m_user(other.m_user), m_host(other.m_host), m_port(other.m_port) {
	m_user = other.m_user;
	m_host = other.m_host;
}

Contact& Contact::operator=(const Contact& other)
{
	m_user = QString(other.m_user);
	m_host = Host(other.m_host);
	return *this;
}
bool Contact::operator==(const Contact& other) const
{
	return m_user == other.m_user && m_host == other.m_host;
}
bool Contact::operator!=(const Contact& other) const
{
	return !(*this == other);
}

QString Contact::user() const
{
	return m_user;
}
Host Contact::host() const
{
	return m_host;
}
QHostAddress Contact::address() const
{
	return m_host.address();
}
QString Contact::hostname() const
{
	return m_host.hostname();
}
quint16 Contact::port() const
{
	return m_host.port();
}

QString Contact::toString() const
{
	return m_user + "@" + m_host.toString();
	if (m_host.port() == Config::DEFAULT_PORT)
		return m_user + "@" + hostname();
	else
		return m_user + "@" + hostname() + ":" + QString::number(m_host.port());
}
QString Contact::id() const
{
	return "Contact<" + m_user + "@" + m_host.id() + ">";
}
QString Contact::print() const
{
	return "Contact " + m_user + "@" + m_host.id();
}
QString Contact::serialize() const
{
	return "Contact<" + m_user + "@" + m_host.id() + ">";
}
Contact Contact::deserialize(QString _str)
{
	StaticID id("Contact::fromId");
	if (_str.startsWith("Contact<") && _str.endsWith(">")) {
		QString str = _str.mid(8);
		str = str.left(str.size() - 1);
		QStringList parts = str.split("@");
		if (parts.size() == 2) {
			Host host = Host::deserialize(parts[1]);
			Contact obj = Contact(parts[0], host);
			id.logger().debug("deserialization successful: %1 = %2", _str, Log::print(obj));
			return obj;
		}
	}
	id.logger().debug("deserialization failed: %1", _str);
	return Contact::INVALID_CONTACT;
}

bool compareContacts(const Contact& left, const Contact& right)
{
	return left.id() < right.id();
}

QDataStream& operator<<(QDataStream& out, const Host& myObj)
{
	out << QString("Host") << myObj.address() << myObj.hostname() << quint32(myObj.port());
	return out;
}
QDataStream & operator>>(QDataStream & in, Host & myObj)
{
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
		myObj = Host::INVALID_HOST;
	}
	return in;
}

QDataStream& operator<<(QDataStream& out, const Contact& myObj)
{
	out << QString("Contact") << myObj.user() << myObj.host();
	return out;
}
QDataStream & operator>>(QDataStream & in, Contact & myObj)
{
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

void fromId(QString _str, Host& obj)
{
	obj = Host::deserialize(_str);
}

void fromId(QString _str, Contact& obj)
{
	obj = Contact::deserialize(_str);
}

