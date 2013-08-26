#include "server.h"
#include "serverconnection.h"
#include "serverconnectionthread.h"
#include "config.h"

Server::Server(QObject* parent)
		: QTcpServer(parent) {
}

void Server::start() {
	listen(QHostAddress::Any, Config::DEFAULT_PORT);
}

void Server::incomingConnection(qintptr socketDescriptor) {
	ServerConnectionThread *thread = new ServerConnectionThread(socketDescriptor, this);
	QObject::connect(thread, &ServerConnectionThread::finished, thread, &ServerConnectionThread::deleteLater);
	thread->start();
}

