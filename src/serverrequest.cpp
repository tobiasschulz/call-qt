/*
 * serverthread.cpp
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#include <QTimer>

#include "serverrequest.h"
#include "connection.h"
#include "contactlist.h"
#include "chatclient.h"

ServerRequest::ServerRequest(int socketDescriptor, Thread* thread, QObject *parent)
		: QObject(parent), socketDescriptor(socketDescriptor), m_socket(0), m_connection(0), m_thread(thread)
{
	log.debug("constuctor()");
}

QString ServerRequest::id() const
{
	return "ServerRequest<" + (m_connection != 0 ? m_connection->id() : "?") + ">";
}
QString ServerRequest::print(PrintFormat format) const
{
	QString data((m_connection != 0 ? m_connection->print(PRINT_ONLY_DATA) : "?"));

	if (format == PRINT_ONLY_NAME)
		return "ServerRequest";
	else if (format == PRINT_ONLY_DATA)
		return data;
	else
		return "ServerRequest " + data;
}

void ServerRequest::start()
{
	log.debug("start()");
	m_socket = new QTcpSocket();
	if (!m_socket->setSocketDescriptor(socketDescriptor)) {
		emit error(m_socket->error());
		return;
	}
	m_connection = new Connection(Connection::SERVER, m_socket);
	QObject::connect(m_connection, &Connection::contactFound, ContactList::instance(), &ContactList::addContact);
	QObject::connect(m_connection, &Connection::disconnected, m_thread, &Thread::quit);
	QObject::connect(m_connection, &Connection::connected, this, &ServerRequest::onConnected);
	QObject::connect(m_thread, &Thread::finished, m_socket, &QTcpSocket::close);
	m_connection->connect(m_socket);

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
}

void ServerRequest::onConnected()
{
	QHash<QString, QString> headers = m_connection->headers();
	QString request = headers["request"];
	log.debug("request = %1", request);
	if (request == "Status") {
		onStatusConnection();
	} else if (request == "Chat") {
		onChatConnection();
	}
}

void ServerRequest::onStatusConnection()
{
	log.debug("onStatusConnection()");
}
void ServerRequest::onChatConnection()
{
	log.debug("onChatConnection()");
	ChatClient* chatclient = new ChatClient(m_connection, m_connection);
}

