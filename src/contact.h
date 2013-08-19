#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QHostAddress>

#include "log.h"

class Contact: public QObject, public Id {
Q_OBJECT
public:
	explicit Contact(QString username, QHostAddress host, quint16 port, QObject* parent = 0);

	QString getUser() const;
	QHostAddress getHost() const;
	quint16 getPort() const;

	QString toString() const;
	QString id() const;

	static const Contact INVALID_CONTACT;

signals:

public slots:

private:
	QString m_user;
	QHostAddress m_host;
	quint16 m_port;
};

#endif // CONTACT_H
