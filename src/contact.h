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
	explicit Host(QObject* parent = 0);
	Host(const Host& other);
	Host& operator=(const Host& other);
	bool operator==(const Host& other) const;
	bool operator!=(const Host& other) const;

	QHostAddress address() const;
	QString hostname() const;
	quint16 port() const;

	bool isReachable() const;
	bool isUnreachable() const;
	bool isLoopback() const;

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

	void lookupHostname();
	void lookupAddress();

	static const Host INVALID_HOST;
	static const QHostAddress INVALID_ADDRESS;
	static const QString INVALID_HOSTNAME;
	static const quint16 INVALID_PORT;

signals:

public slots:

private:
	QHostAddress m_address;
	bool m_address_valid;
	QString m_hostname;
	bool m_hostname_valid;
	quint16 m_port;
};

class Contact: public QObject, public ID
{
Q_OBJECT
public:
	explicit Contact(QString username, QHostAddress host, quint16 port, QObject* parent = 0);
	explicit Contact(QString username, Host host, QObject* parent = 0);
	explicit Contact(QObject* parent = 0);
	Contact(const Contact& other);
	Contact& operator=(const Contact& other);
	bool operator==(const Contact& other) const;
	bool operator!=(const Contact& other) const;

	QString user() const;
	Host host() const;
	QHostAddress address() const;
	QString hostname() const;
	quint16 port() const;

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
	Host m_host;
};

Q_DECLARE_METATYPE(Host);
Q_DECLARE_METATYPE(Contact);

bool compareContacts(const Contact& left, const Contact& right);

QDataStream &operator<<(QDataStream &out, const Host& myObj);
QDataStream &operator>>(QDataStream &in, Host& myObj);
QDataStream &operator<<(QDataStream &out, const Contact& myObj);
QDataStream &operator>>(QDataStream &in, Contact& myObj);

void fromId(QString str, Host& obj);
void fromId(QString str, Contact& obj);

#endif // CONTACT_H
