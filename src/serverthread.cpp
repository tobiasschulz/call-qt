/*
 * serverthread.cpp
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#include "serverthread.h"
#include "server.h"

ServerThread::ServerThread(QObject *parent)
		: QThread(parent)
{
}

void ServerThread::run()
{
	Server server;
	server.start();

	exec();
}

