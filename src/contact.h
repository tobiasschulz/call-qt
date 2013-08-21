#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QHostAddress>

#include "log.h"

class Contact: public QObject, public Id {
Q_OBJECT
public:
	explicit Contact(QString username, QHostAddress host, quint16 port, QObject* parent = 0);
	explicit Contact(QObject* parent = 0);
	Contact(const Contact& other);
	Contact& operator=(const Contact& other);
	bool operator==(const Contact& other) const;

	QString user() const;
	QHostAddress host() const;
	QString hostname() const;
	quint16 port() const;

	QString toString() const;
	QString id() const;

	static const Contact INVALID_CONTACT;
	static const QString INVALID_USER;
	static const QHostAddress INVALID_HOST;
	static const quint16 INVALID_PORT;

signals:

public slots:

private:
	QString m_user;
	QHostAddress m_host;
	QString m_hostname;
	quint16 m_port;
};


uint qHash(const Contact& c);
bool compareContacts(const Contact& left, const Contact& right);

#endif // CONTACT_H
