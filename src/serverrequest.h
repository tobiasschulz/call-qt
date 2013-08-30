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
#include <QPointer>

#include "thread.h"
#include "connection.h"
#include "id.h"

class ServerRequest: public QObject, public ID
{
Q_OBJECT

public:
	ServerRequest(int socketDescriptor, Thread* thread, QObject *parent = 0);

	void onStatusConnection();
	void onChatConnection();
	void onCallConnection();
	void openChatTab();

	QString id() const;
	QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;

signals:
	void error(QTcpSocket::SocketError socketError);
	void openContactTab(Contact contact);

public slots:
	void start();
	void onConnected();
	void onChatTabOpened(Contact contact);

private:
	int socketDescriptor;
	QPointer<Connection> m_connection;
	QPointer<Thread> m_thread;
};

#endif /* SERVERREQUEST_H */
