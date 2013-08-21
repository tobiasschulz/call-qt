#ifndef STATUSCONNECTION_H
#define STATUSCONNECTION_H

#include <QObject>

#include "connection.h"

class StatusConnection: public Connection {
Q_OBJECT

public:
	StatusConnection(const Contact& contact, QObject* parent = 0);
	StatusConnection(const Host& host, QObject* parent = 0);
	virtual QString id() const;

signals:

public slots:

};

#endif // STATUSCONNECTION_H
