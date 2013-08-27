/*
 * serverrequest.h
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#ifndef SERVERREQUEST_H
#define SERVERREQUEST_H

#include <QThread>
#include <QTcpSocket>

#include "thread.h"
#include "connection.h"
#include "log.h"

class ServerRequest: public QObject, public ID
{
Q_OBJECT

public:
	ServerRequest(int socketDescriptor, Thread* thread, QObject *parent = 0);

	void onStatusConnection();
	void onChatConnection();

	QString id() const;
	QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;

signals:
	void error(QTcpSocket::SocketError socketError);

public slots:
	void onConnected();
	void start();

private:
	int socketDescriptor;
	QTcpSocket* m_socket;
	Connection* m_connection;
	Thread* m_thread;
};

#endif /* SERVERREQUEST_H */
