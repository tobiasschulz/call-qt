#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>

class ServerConnection;

class Server: public QTcpServer {
Q_OBJECT

public:
	Server(QObject* parent = 0);

signals:

protected:
	void incomingConnection(qintptr socketDescriptor);

};

#endif // SERVER_H
