#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <QTime>
#include <QTimer>

#include "contact.h"
#include "connection.h"

static const int MaxBufferSize = 1024;

class ServerConnection: public Connection
{
Q_OBJECT

public:
	ServerConnection(QTcpSocket* socket, QObject* parent = 0);
	virtual QString id() const;

signals:

public slots:

private:

};

#endif // SERVERCONNECTION_H
