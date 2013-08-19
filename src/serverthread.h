/*
 * serverthread.h
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>

class ServerThread: public QThread {
Q_OBJECT

public:
	ServerThread(int socketDescriptor, QObject *parent);

	void run();

signals:
	void error(QTcpSocket::SocketError socketError);

private:
	int socketDescriptor;
};

#endif /* SERVERTHREAD_H */
