/*
 * serverthread.cpp
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#include "serverthread.h"
#include "serverconnection.h"

ServerThread::ServerThread(int socketDescriptor, QObject *parent)
		: QThread(parent), socketDescriptor(socketDescriptor) {
}

void ServerThread::run() {
	QTcpSocket* socket = new QTcpSocket();
	if (!socket->setSocketDescriptor(socketDescriptor)) {
		emit error(socket->error());
		return;
	}
	ServerConnection* connection = new ServerConnection(socket, socket);

	/*
	 QByteArray block;
	 QDataStream out(&block, QIODevice::WriteOnly);
	 out.setVersion(QDataStream::Qt_4_0);
	 out << (quint16) 0;
	 out << text;
	 out.device()->seek(0);
	 out << (quint16) (block.size() - sizeof(quint16));

	 socket->write(block);
	 socket->disconnectFromHost();
	 socket->waitForDisconnected();
	 */
	socket->waitForDisconnected();
}

