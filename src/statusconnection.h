#ifndef STATUSCONNECTION_H
#define STATUSCONNECTION_H

#include <QObject>

#include "connection.h"

class StatusConnection: public Connection {
Q_OBJECT

public:
	StatusConnection(QObject* parent, Contact contact);
	StatusConnection(QObject* parent, QHostAddress host, quint16 port);signals:

public slots:

};

#endif // STATUSCONNECTION_H
