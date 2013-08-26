/*
 * serverconnectionthread.cpp
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#include "serverconnectionthread.h"
#include "connection.h"
#include "contactlist.h"

ServerConnectionThread::ServerConnectionThread(int socketDescriptor, QObject *parent)
		: QThread(parent), socketDescriptor(socketDescriptor)
{
}

void ServerConnectionThread::run()
{
	QTcpSocket* socket = new QTcpSocket();
	if (!socket->setSocketDescriptor(socketDescriptor)) {
		emit error(socket->error());
		return;
	}
	Connection* connection = new Connection(Connection::SERVER, socket);
	connection->connect(socket);
	QObject::connect(connection, &Connection::contactFound, ContactList::instance(),
			&ContactList::addContact);
	QObject::connect(connection, &Connection::disconnected, this, &ServerConnectionThread::quit);
	QObject::connect(this, &ServerConnectionThread::finished, socket, &QTcpSocket::close);

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
	exec();
}

