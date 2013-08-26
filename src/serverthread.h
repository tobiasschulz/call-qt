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
	ServerThread(QObject *parent = 0);

	void run();
};

#endif /* SERVERTHREAD_H */
