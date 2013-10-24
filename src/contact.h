#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QHostAddress>
#include <QHostInfo>

#include "id.h"

class Host: public QObject, public ID
{
Q_OBJECT
public:
	explicit Host(QHostAddress address, quint16 port, QObject* parent = 0);
	explicit Host(QString hostname, quint16 port, QObject* parent = 0);
	explicit Host(QHostAddress address, QString hostname, quint16 port, QObject* parent = 0);
	explicit Host(QObject* parent = 0);
	Host(const Host& other);
	Host& operator=(const Host& other);
	bool operator==(const Host& other) const;
	bool operator!=(const Host& other) const;

	QHostAddress address() const;
	QString hostname() const;
	QString hostname();
	quint16 port() const;
	QString displayname() const;
	QStringList displaynames() const;

	bool isReachable() const;
	bool isUnreachable() const;
	bool isLoopback() const;
	bool isDynamicIP() const;
	bool isWanIP() const;
	bool isLanIP() const;
	bool isLocalIP() const;

	enum PortFormat
	{
		SHOW_PORT_ALWAYS, SHOW_PORT_ONLY_UNUSUAL
	};
	enum HostFormat
	{
		SHOW_ADDRESS, SHOW_HOSTNAME
	};
	QString toString(PortFormat showPort = SHOW_PORT_ONLY_UNUSUAL, HostFormat hostFormat = SHOW_HOSTNAME) const;
	QString id() const;
	QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;
	QString serialize() const;
	static Host deserialize(QString str);

	static const Host INVALID_HOST;
	static const QHostAddress INVALID_ADDRESS;
	static const QString INVALID_HOSTNAME;
	static const quint16 INVALID_PORT;

signals:

public slots:

private:
	enum FieldState
	{
		INITIAL, LOOKED_UP, INVALID, LOOKUP_PENDING
	};

	void lookupHostname();
	void lookupHostname(QString* hostname, FieldState* state) const;
	void lookupAddress();

	QHostAddress m_address;
	FieldState m_address_state;
	QString m_hostname;
	FieldState m_hostname_state;
	quint16 m_port;
};

class Contact: public QObject, public ID
{
Q_OBJECT
public:
	explicit Contact(QString username, QString computername, Host host, QObject* parent = 0);
	explicit Contact(QObject* parent = 0);
	Contact(const Contact& other);
	Contact& operator=(const Contact& other);
	bool operator==(const Contact& other) const;
	bool operator!=(const Contact& other) const;

	QString user() const;
	QString computername() const;
	Host host() const;
	QHostAddress address() const;
	QString hostname() const;
	quint16 port() const;
	QString displayname() const;

	void invalidate();
	QString toString() const;
	QString id() const;
	QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;
	QString serialize() const;
	static Contact deserialize(QString str);

	static const Contact INVALID_CONTACT;
	static const QString INVALID_USER;

signals:

public slots:

private:
	QString m_user;
	QString m_computername;
	Host m_host;
};

Q_DECLARE_METATYPE(Host);
Q_DECLARE_METATYPE(Contact);

bool compareContacts(const Contact& left, const Contact& right);
bool compareHostnamesAndAddresses(const QString& left, const QString& right);

QDataStream &operator<<(QDataStream &out, const Host& myObj);
QDataStream &operator>>(QDataStream &in, Host& myObj);
QDataStream &operator<<(QDataStream &out, const Contact& myObj);
QDataStream &operator>>(QDataStream &in, Contact& myObj);

void fromId(QString str, Host& obj);
void fromId(QString str, Contact& obj);

#endif // CONTACT_H
