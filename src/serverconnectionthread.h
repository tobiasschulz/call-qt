/*
 * serverconnectionthread.h
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#ifndef SERVERCONNECTIONTHREAD_H
#define SERVERCONNECTIONTHREAD_H

#include <QThread>
#include <QTcpSocket>

#include "thread.h"

class ServerConnectionThread: public Thread
{
Q_OBJECT

public:
	ServerConnectionThread(int socketDescriptor, QObject *parent);

	void run();

signals:
	void error(QTcpSocket::SocketError socketError);

private:
	int socketDescriptor;
};

#endif /* SERVERCONNECTIONTHREAD_H */
