#ifndef STATUSCONNECTION_H
#define STATUSCONNECTION_H

#include <QObject>

#include "connection.h"

class PingClient: public QObject, public Id {
Q_OBJECT

public:
	PingClient(const Contact& contact, QObject* parent = 0);
	PingClient(const Host& host, QObject* parent = 0);
	virtual QString id() const;

signals:

public slots:
	void ping();

private:
	Host m_host;
	Connection* m_connection;
};

#endif // STATUSCONNECTION_H
