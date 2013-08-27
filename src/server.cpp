#include <QTimer>

#include "server.h"
#include "serverrequest.h"
#include "thread.h"
#include "config.h"

Server::Server(QObject* parent)
		: QTcpServer(parent)
{
}

void Server::start()
{
	listen(QHostAddress::Any, Config::DEFAULT_PORT);
}

void Server::incomingConnection(qintptr socketDescriptor)
{
	Thread* thread = new Thread("ServerReq");
	thread->start();
	ServerRequest* request = new ServerRequest(socketDescriptor, thread);
	request->moveToThread(thread);
	QObject::connect(thread, &Thread::finished, thread, &Thread::deleteLater);
	QObject::connect(thread, &Thread::finished, request, &ServerRequest::deleteLater);
	QTimer::singleShot(0, request, SLOT(start()));
}

