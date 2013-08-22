#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QHostAddress>

#include "log.h"

class Host: public QObject, public Id {
Q_OBJECT
public:
	explicit Host(QHostAddress host, quint16 port, QObject* parent = 0);
	explicit Host(QObject* parent = 0);
	Host(const Host& other);
	Host& operator=(const Host& other);
	bool operator==(const Host& other) const;

	QHostAddress address() const;
	QString hostname() const;
	quint16 port() const;

	QString toString() const;
	QString id() const;

	static const QHostAddress INVALID_ADDRESS;
	static const QString INVALID_HOSTNAME;
	static const quint16 INVALID_PORT;

signals:

public slots:

private:
	QHostAddress m_address;
	QString m_hostname;
	quint16 m_port;
};

class Contact: public QObject, public Id {
Q_OBJECT
public:
	explicit Contact(QString username, QHostAddress host, quint16 port, QObject* parent = 0);
	explicit Contact(QString username, Host host, QObject* parent = 0);
	explicit Contact(QObject* parent = 0);
	Contact(const Contact& other);
	Contact& operator=(const Contact& other);
	bool operator==(const Contact& other) const;

	QString user() const;
	Host host() const;
	QHostAddress address() const;
	QString hostname() const;
	quint16 port() const;

	QString toString() const;
	QString id() const;

	static const Contact INVALID_CONTACT;
	static const QString INVALID_USER;
	static const Host INVALID_HOST;

signals:

public slots:

private:
	QString m_user;
	Host m_host;
};

bool compareContacts(const Contact& left, const Contact& right);

QDataStream &operator<<(QDataStream &out, const Host &myObj);
QDataStream &operator>>(QDataStream &in, Host &myObj);

QDataStream &operator<<(QDataStream &out, const Contact &myObj);
QDataStream &operator>>(QDataStream &in, Contact &myObj);

#endif // CONTACT_H
