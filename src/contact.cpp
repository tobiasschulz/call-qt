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

bool isAddressValid(QHostAddress address)
{
	return address.protocol() == QAbstractSocket::IPv4Protocol || address.protocol() == QAbstractSocket::IPv6Protocol;
}

QHostAddress lookupAddress(DnsCache::LookupMode mode, QString hostname, QHostAddress defaultAddress)
{
	QHostInfo info = DnsCache::instance()->lookup(hostname, mode);
	if (info.addresses().size() > 0) {
		return info.addresses().first();
	}
	return defaultAddress;
}

Host::Host(QHostAddress address, quint16 port, QObject* parent)
		: QObject(parent), m_address(address), m_hostname(INVALID_HOSTNAME), m_port(port)
{
	m_address_state = isAddressValid(address) ? VALID : INVALID;
	m_reachability = updateReachability();
}
Host::Host(QString hostname, quint16 port, QObject* parent)
		: QObject(parent), m_port(port)
{
	QHostAddress address(hostname);
	if (isAddressValid(address)) {
		m_address = address;
		m_address_state = VALID;
		m_hostname = INVALID_HOSTNAME;
	} else {
		m_address = lookupAddress(DnsCache::CACHE_LOOKUP, hostname, INVALID_ADDRESS);
		m_address_state = LOOKUP_PENDING;
		m_hostname = hostname;
	}
	m_reachability = updateReachability();
}
Host::Host(QObject * parent)
		: QObject(parent), m_address(INVALID_ADDRESS), m_address_state(INVALID), m_hostname(INVALID_HOSTNAME), m_port(0)
{
	m_reachability = updateReachability();
}
Host::Host(const Host& other)
		: QObject(other.parent())
{
	m_address = QHostAddress(other.m_address);
	m_address_state = other.m_address_state;
	m_hostname = QString(other.m_hostname);
	m_port = other.m_port;
	m_reachability = other.m_reachability;
}

Host& Host::operator=(const Host& other)
{
	m_address = QHostAddress(other.m_address);
	m_address_state = other.m_address_state;
	m_hostname = QString(other.m_hostname);
	m_port = other.m_port;
	m_reachability = other.m_reachability;
	return *this;
}
bool Host::operator==(const Host& other) const
{
	return (displayname() == other.displayname() && m_port == other.m_port)
			|| (displayname() == other.displayname() && isUnreachable() == other.isUnreachable());
}
bool Host::operator!=(const Host& other) const
{
	return !(*this == other);
}

QHostAddress Host::address() const
{
	if (m_address_state == LOOKUP_PENDING) {
		return lookupAddress(DnsCache::BLOCKING_LOOKUP, m_hostname, m_address);
	} else {
		return m_address;
	}
}
QHostAddress Host::address()
{
	if (m_address_state == LOOKUP_PENDING) {
		return m_address = lookupAddress(DnsCache::BLOCKING_LOOKUP, m_hostname, m_address);
	} else {
		return m_address;
	}
}
QString Host::displayname() const
{
	switch (m_address_state) {
	case VALID:
		return m_address.toString();
	case LOOKUP_PENDING:
		return m_hostname;
	default:
		return "";
	}
}
QStringList Host::displaynames() const
{
	QStringList names;
	if (m_address_state == VALID) {
		names << m_address.toString();
	}
	if (!m_hostname.isEmpty()) {
		names << m_hostname;
	}
	return names;
}
quint16 Host::port() const
{
	return m_port;
}

Host::HostReachability Host::reachability() const
{
	return m_reachability;
}
bool Host::isReachable() const
{
	return m_reachability == REACHABLE;
}
bool Host::isUnreachable() const
{
	return m_reachability == UNREACHABLE;
}

Host::HostReachability Host::updateReachability()
{
	{
		static bool fuck = true;
		if (fuck == true) {
			fuck = false;
			if (!isReachable() && Config::instance()->isHost(*this, Config::LOCALHOST))
				log.debug("isUnreachable()=%1, isHost(*this,LOCALHOST)=%2", QString::number(!isReachable()),
						QString::number(Config::instance()->isHost(*this, Config::LOCALHOST)));
			fuck = true;
		}
	}

	if (m_port < 32768) {
		return REACHABLE;
	} else {
		return UNREACHABLE;
	}
}

Host::HostScope Host::scope() const
{
	QString address_str = address().toString();
	return address_str.startsWith("127.") ? LOCAL_SCOPE :
			(address_str.startsWith("10.") || address_str.startsWith("192.168.")) ? LAN_SCOPE : WAN_SCOPE;
}
bool Host::isDynamicIP() const
{
	return scope() == WAN_SCOPE;
}

QString Host::toString() const
{
	return print(PRINT_ONLY_DATA);
}
QString Host::id() const
{
	return "Host<" + print(PRINT_ONLY_DATA) + ">";
}
QString Host::print(PrintFormat format) const
{
	QString data;
	if (isUnreachable())
		data = displayname() + ":incoming";
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
	return "Host<" + address().toString() + ":" + QString::number(m_port) + ">";
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
			if (parts[0].isEmpty()) {
				Host obj = Host(QHostAddress(parts[1]), parts[2].toInt());
				id.logger().debug("deserialization successful: %1 = %2", _str, Log::print(obj));
				return obj;
			} else {
				Host obj = Host(parts[0], parts[2].toInt());
				id.logger().debug("deserialization successful: %1 = %2", _str, Log::print(obj));
				return obj;
			}
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

QList<Contact> User::contacts() const
{
	QList<Contact> contacts;
	foreach (const Host& host, m_hosts[*this])
	{
		Contact c(*this, host);
		if (c != Contact::INVALID_CONTACT) {
			contacts << c;
		}
	}
	return contacts;
}
QList<Host> User::hosts() const
{
	return m_hosts[*this];
}
void User::addHost(const Host& host) const
{
	if (host != Host::INVALID_HOST) {
		QList<Host> hosts = m_hosts[*this];
		if (!hosts.contains(host)) {
			hosts << host;
			qSort(hosts.begin(), hosts.end(), compareHosts);
			m_hosts[*this] = hosts;
		}
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
	user.addHost(host);
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
QString Contact::displayname() const
{
	return m_host.displayname();
}
quint16 Contact::port() const
{
	return m_host.port();
}

Contact Contact::reachableContact() const
{
	if (host().isUnreachable()) {
		foreach (const Contact& contact, user().contacts())
		{
			if (contact.host().address() == host().address() && contact.host().isReachable()) {
				return contact;
			}
		}
		foreach (const Contact& contact, user().contacts())
		{
			if (contact.host().isReachable()) {
				return contact;
			}
		}
	}
	return *this;
}

bool Contact::isMe() const
{
	return username() == SystemUtil::instance()->getUserName()
			&& computername() == SystemUtil::instance()->getComputerName();
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
	out << QString("Host") << myObj.address() << quint32(myObj.port());
	return out;
}
QDataStream & operator>>(QDataStream & in, Host & myObj)
{
	QHostAddress address;
	quint32 port;
	QString type;
	in >> type;
	if (type == "Host") {
		in >> address >> port;
		myObj = Host(address, port);
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

