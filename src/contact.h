#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QHostAddress>

class Contact: public QObject {
Q_OBJECT
public:
	explicit Contact(QObject* parent, QString username, QHostAddress host, quint16 port);

	QString getUser() const;
	QHostAddress getHost() const;
	quint16 getPort() const;

	QString toString() const;

	static const Contact INVALID_CONTACT;

signals:

public slots:

private:
	QString m_user;
	QHostAddress m_host;
	quint16 m_port;
};

#endif // CONTACT_H
