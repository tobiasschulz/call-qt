#include "server.h"
#include "serverconnection.h"

Server::Server(QObject* parent)
		: QTcpServer(parent) {
	listen(QHostAddress::Any);
}

void Server::incomingConnection(qintptr socketDescriptor) {
	ServerConnection* connection = new ServerConnection(this);
	QTcpSocket* socket = new QTcpSocket(connection);
	socket->setSocketDescriptor(socketDescriptor);
	connection->setSocket(socket);
	emit newConnection(connection);
}
