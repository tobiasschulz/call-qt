#include <QThread>
#include <QTimer>
#include <QRegExp>

#include "contact.h"
#include "contactlist.h"
#include "networkutil.h"
#include "systemutil.h"
#include "dnscache.h"
#include "config.h"

const QHostAddress Host::INVALID_ADDRESS;
const QString Host::INVALID_HOSTNAME;
const quint16 Host::INVALID_PORT = 0;
const Host Host::INVALID_HOST;

const QString User::INVALID_USERNAME;
const QString User::INVALID_COMPUTERNAME;
const User User::INVALID_USER;
QHash<User, QList<Host>> User::m_hosts;

const Contact Contact::INVALID_CONTACT;

Host::Host(QHostAddress address, quint16 port, QObject* parent)
		: QObject(parent), m_address(address), m_address_state(INITIAL), m_hostname(), m_hostname_state(LOOKUP_PENDING),
			m_port(port)
{
	//lookupHostname();
}
Host::Host(QString hostname, quint16 port, QObject* parent)
		: QObject(parent), m_address(), m_address_state(LOOKUP_PENDING), m_hostname(), m_hostname_state(LOOKUP_PENDING),
			m_port(port)
{
	QHostAddress address(hostname);
	if (address.protocol() == QAbstractSocket::IPv4Protocol || address.protocol() == QAbstractSocket::IPv6Protocol) {
		m_address = address;
		m_address_state = INITIAL;
		//log.debug("Valid IPv4 address: %1", m_address.toString());
		//lookupHostname();
	} else {
		m_hostname = hostname;
		m_hostname_state = INITIAL;
		//log.debug("Got a hostname: %1", hostname);
		lookupAddress();
	}
}
Host::Host(QHostAddress address, QString hostname, quint16 port, QObject* parent)
		: QObject(parent), m_address(address), m_address_state(INITIAL), m_hostname(hostname), m_port(port)
{
	m_hostname_state = m_hostname.size() > 0 ? INITIAL : INVALID;
}
Host::Host(QObject * parent)
		: QObject(parent), m_address(), m_address_state(INVALID), m_hostname(), m_hostname_state(INVALID), m_port(0)
{
}
Host::Host(const Host& other)
		: QObject(other.parent()), m_address(), m_address_state(), m_hostname(), m_hostname_state(), m_port(0)
{
	m_address = QHostAddress(other.m_address);
	m_hostname = QString(other.m_hostname);
	m_port = other.m_port;
	m_address_state = other.m_address_state;
	m_hostname_state = other.m_hostname_state;
}

Host& Host::operator=(const Host& other)
{
	m_address = QHostAddress(other.m_address);
	m_hostname = QString(other.m_hostname);
	m_port = other.m_port;
	m_address_state = other.m_address_state;
	m_hostname_state = other.m_hostname_state;
	return *this;
}
bool Host::operator==(const Host& other) const
{
	return (displayname() == other.displayname() && m_port == other.m_port) || (isLoopback() && other.isLoopback());
}
bool Host::operator!=(const Host& other) const
{
	return !(*this == other);
}

QHostAddress Host::address() const
{
	return m_address;
}
QString Host::displayname() const
{
	if (m_address_state == INITIAL) {
		return m_address.toString();
	} else if (m_hostname_state == INITIAL) {
		return m_hostname;
	} else {
		return "";
	}
}
QStringList Host::displaynames() const
{
	QStringList names;
	if (m_address_state != INVALID && m_address.toString().size() != 0) {
		names << m_address.toString();
	}
	if (m_hostname_state != INVALID && m_hostname.size() != 0) {
		names << m_hostname;
	}
	return names;
}
QString Host::hostname() const
{
	if (m_hostname_state == LOOKUP_PENDING) {
		QString hostname;
		FieldState state;
		lookupHostname(&hostname, &state);
		return hostname;
	} else {
		return m_hostname;
	}
}
QString Host::hostname()
{
	if (m_hostname_state == LOOKUP_PENDING) {
		lookupHostname();
		return m_hostname;
	} else {
		return m_hostname;
	}
}
quint16 Host::port() const
{
	return m_port;
}
bool Host::isReachable() const
{
	return m_port < 10000;
}
bool Host::isUnreachable() const
{
	return !isReachable();
}
bool Host::isLoopback() const
{
	return !isReachable() && Config::instance()->isHostname(m_address.toString(), Config::LOCALHOST);
}
bool Host::isDynamicIP() const
{
	return scope() == WAN;
}
Host::HostScope Host::scope() const
{
	return m_address.toString().startsWith("127.") ? LOCAL :
			(m_address.toString().startsWith("10.") || m_address.toString().startsWith("192.168.")) ? LAN : WAN;

}
void Host::lookupAddress()
{
	QHostInfo info = DnsCache::instance()->lookup(m_hostname, DnsCache::BLOCK_IF_NEEDED);
	if (info.addresses().size() != 0) {
		m_address = info.addresses().first();
		m_address_state = LOOKED_UP;
		//log.debug("lookupAddress(%1) = %2", m_hostname, m_address.toString());
	} else {
		m_address = INVALID_ADDRESS;
		m_address_state = INVALID;
		log.debug("lookupAddress(%1) failed! no address found!", m_hostname);
	}
}
void Host::lookupHostname()
{
	lookupHostname(&m_hostname, &m_hostname_state);
}
void Host::lookupHostname(QString* hostname, FieldState* state) const
{
	if (m_address_state == INVALID || m_address.toString().size() == 0) {
		*hostname = INVALID_HOSTNAME;
		*state = INVALID;
	} else {
		QHostInfo info = DnsCache::instance()->lookup(m_address.toString(), DnsCache::CACHE_ONLY, this);

		if (info.hostName().size() > 0) {
			*hostname = info.hostName();
			*state = LOOKED_UP;
			// log.debug("lookupAddress(%1) = %2", m_address.toString(), m_hostname);
		} else {
			*hostname = INVALID_HOSTNAME;
			*state = INVALID;
			log.debug("lookupHostname(%1) failed! no hostname found!", m_address.toString());
		}
	}
}

QString Host::toString(PortFormat showPort, HostFormat hostFormat) const
{
	QString formattedHost = hostFormat == SHOW_HOSTNAME ? displayname() : address().toString();
	if (isLoopback())
		return (hostFormat == SHOW_HOSTNAME ? "loopback device" : "loopback") + QString(" (") + formattedHost + ":"
				+ QString::number(m_port) + ")";
	else if (m_port == Config::instance()->DEFAULT_PORT && showPort == SHOW_PORT_ONLY_UNUSUAL)
		return formattedHost;
	else
		return formattedHost + ":" + QString::number(m_port);
}
QString Host::id() const
{
	if (isLoopback())
		return "Host<loopback>";
	else
		return "Host<" + address().toString() + ":" + QString::number(m_port) + ">";
}
QString Host::print(PrintFormat format) const
{
	QString data;
	if (isLoopback())
		data = "loopback";
	else
		data = displayname() + ":" + QString::number(m_port);

	if (format == PRINT_ONLY_NAME)
		return "Host";
	else if (format == PRINT_ONLY_DATA)
		return data;
	else
		return "Host " + data;
}
QString Host::serialize() const
{
	return "Host<" + m_hostname + "~" + address().toString() + ":" + QString::number(m_port) + ">";
}
Host Host::deserialize(QString _str)
{
	StaticID id("Host::fromId");
	if (_str.startsWith("Host<") && _str.endsWith(">")) {
		QString str = _str.mid(5);
		str = str.left(str.size() - 1);
		QStringList parts = str.split(QRegExp("[:~]"), QString::SkipEmptyParts);
		// foreach (QString p, parts) id.logger().debug("deserialization: %1", p);
		if (parts.size() == 2) {
			Host obj = Host(QHostAddress(parts[0]), parts[1].toInt());
			id.logger().debug("deserialization successful: %1 = %2", _str, Log::print(obj));
			return obj;
		}
		if (parts.size() == 3) {
			Host obj = Host(QHostAddress(parts[1]), parts[0], parts[2].toInt());
			id.logger().debug("deserialization successful: %1 = %2", _str, Log::print(obj));
			return obj;
		}
	}
	id.logger().debug("deserialization failed: %1", _str);
	return Host::INVALID_HOST;
}

User::User(QString username, QString computername, QObject* parent)
		: QObject(parent), m_username(username), m_computername(computername)
{
	if (username.size() == 0) {
		m_username = INVALID_USERNAME;
	}
}
User::User(QObject * parent)
		: QObject(parent), m_username(INVALID_USERNAME), m_computername()
{
}
User::User(const User& other)
		: QObject(other.parent())
{
	m_username = other.m_username;
	m_computername = other.m_computername;
}

User& User::operator=(const User& other)
{
	m_username = QString(other.m_username);
	m_computername = QString(other.m_computername);
	return *this;
}
bool User::operator==(const User& other) const
{
	return m_username == other.m_username && m_computername == other.m_computername;
}
bool User::operator!=(const User& other) const
{
	return !(*this == other);
}

QString User::username() const
{
	return m_username;
}
QString User::computername() const
{
	return m_computername;
}

QList<Host> User::hosts() const
{
	return m_hosts[*this];
}
void User::addHost(const Host& host) const
{
	QList<Host> hosts = m_hosts[*this];
	if (!hosts.contains(host)) {
		hosts << host;
		qSort(hosts.begin(), hosts.end(), compareHosts);
		m_hosts[*this] = hosts;
	}
}

QString User::toString() const
{
	return m_username + "@" + m_computername;
}
QString User::id() const
{
	return "User<" + m_username + "@" + m_computername + ">";
}
QString User::print(PrintFormat format) const
{
	QString data(m_username + "@" + m_computername);
	if (format == PRINT_ONLY_NAME)
		return "User";
	else if (format == PRINT_ONLY_DATA)
		return data;
	else
		return "User " + data;
}
QString User::serialize() const
{
	return "User<" + m_username + "@" + m_computername + ">";
}
User User::deserialize(QString _str)
{
	StaticID id("User::fromId");
	if (_str.startsWith("User<") && _str.endsWith(">")) {
		QString str = _str.mid(5);
		str = str.left(str.size() - 1);
		QStringList parts = str.split(QRegExp("[@=]"));
		if (parts.size() == 2) {
			User obj = User(parts[0], parts[1]);
			id.logger().debug("deserialization successful: %1 = %2", _str, Log::print(obj));
			return obj;
		}
	}
	id.logger().debug("deserialization failed: %1", _str);
	return User::INVALID_USER;
}

Contact::Contact(User user, Host host, QObject* parent)
		: QObject(parent), m_user(user), m_host(host)
{
	if (host.isLoopback() && user.username().toLower() != SystemUtil::instance()->getUserName().toLower()) {
		invalidate();
	} else {
		user.addHost(host);
	}
}
Contact::Contact(QObject * parent)
		: QObject(parent), m_user(User::INVALID_USER), m_host()
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
	m_user = User(other.m_user);
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

void Contact::invalidate()
{
	m_user = User::INVALID_USER;
	m_host = Host::INVALID_HOST;
}

User Contact::user() const
{
	return m_user;
}
QString Contact::username() const
{
	return m_user.username();
}
QString Contact::computername() const
{
	return m_user.computername();
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
QString Contact::displayname() const
{
	return m_host.displayname();
}
quint16 Contact::port() const
{
	return m_host.port();
}

QString Contact::toString() const
{
	return m_user.print(ID::PRINT_ONLY_DATA) + "=" + m_host.toString();
	if (m_host.port() == Config::instance()->DEFAULT_PORT)
		return m_user.print(ID::PRINT_ONLY_DATA) + "=" + displayname();
	else
		return m_user.print(ID::PRINT_ONLY_DATA) + "=" + displayname() + ":" + QString::number(m_host.port());
}
QString Contact::id() const
{
	if (m_host.isLoopback())
		return "Contact<" + m_user.print(ID::PRINT_ONLY_DATA) + "=loopback>";
	else
		return "Contact<" + m_user.print(ID::PRINT_ONLY_DATA) + "=" + m_host.print(ID::PRINT_ONLY_DATA) + ">";
}
QString Contact::print(PrintFormat format) const
{
	QString data(m_user.print(ID::PRINT_ONLY_DATA) + "=" + m_host.print(ID::PRINT_ONLY_DATA));
	if (format == PRINT_ONLY_NAME)
		return "Contact";
	else if (format == PRINT_ONLY_DATA)
		return data;
	else
		return "Contact " + data;
}
QString Contact::serialize() const
{
	return "Contact<" + m_user.print(ID::PRINT_ONLY_DATA) + "=" + m_host.id() + ">";
}
Contact Contact::deserialize(QString _str)
{
	StaticID id("Contact::fromId");
	if (_str.startsWith("Contact<") && _str.endsWith(">")) {
		QString str = _str.mid(8);
		str = str.left(str.size() - 1);
		QStringList parts = str.split(QRegExp("[=]"));
		if (parts.size() == 2) {
			Host host = Host::deserialize(parts[1]);
			User user = User::deserialize(parts[0]);
			Contact obj = Contact(user, host);
			id.logger().debug("deserialization successful: %1 = %2", _str, Log::print(obj));
			return obj;
		}
	}
	id.logger().debug("deserialization failed: %1", _str);
	return Contact::INVALID_CONTACT;
}

bool Host::operator>(const Host& other) const
{
	return *this < other;
}
bool Host::operator<(const Host& other) const
{
	if (scope() == other.scope()) {
		return id() < other.id();
	} else {
		return scope() < other.scope();
	}
}

bool compareHosts(const Host& left, const Host& right)
{
	return left < right;
}

bool compareUsers(const User& left, const User& right)
{
	return left.id() < right.id();
}

bool compareContacts(const Contact& left, const Contact& right)
{
	return left.id() < right.id();
}

bool compareHostnamesAndAddresses(const QString& left, const QString& right)
{
	if (left.size() > 0 && right.size() > 0) {
		if (left.at(0).isDigit() != right.at(0).isDigit()) {
			return !left.at(0).isDigit();
		} else {
			return left < right;
		}
	} else {
		return left < right;
	}
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

QDataStream& operator<<(QDataStream& out, const User& myObj)
{
	out << QString("User") << myObj.username() << myObj.computername();
	return out;
}
QDataStream & operator>>(QDataStream & in, User & myObj)
{
	QString username;
	QString computername;
	QString type;
	in >> type;
	if (type == "User") {
		in >> username >> computername;
		myObj = User(username, computername);
	} else {
		qDebug() << "Error in deserialization of type User: invalid type '" + type + "'!";
		myObj = User::INVALID_USER;
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
	User user;
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

void fromId(QString _str, User& obj)
{
	obj = User::deserialize(_str);
}

void fromId(QString _str, Contact& obj)
{
	obj = Contact::deserialize(_str);
}

