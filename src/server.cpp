#include "server.h"
#include "serverthread.h"
#include "serverconnection.h"
#include "config.h"

Server::Server(QObject* parent)
		: QTcpServer(parent) {
	listen(QHostAddress::Any, Config::DEFAULT_PORT);
}

void Server::incomingConnection(qintptr socketDescriptor) {
	ServerThread *thread = new ServerThread(socketDescriptor, this);
	QObject::connect(thread, &ServerThread::finished, thread, &ServerThread::deleteLater);
	thread->start();
}

