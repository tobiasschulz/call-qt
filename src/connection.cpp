#include "connection.h"

Connection::Connection(QObject* parent, QTcpSocket* socket)
		: QObject(parent), m_socket(0), m_contact(0) {
	setSocket(socket);
}

Connection::Connection(QObject* parent, Contact contact)
		: QObject(parent), m_socket(0), m_contact(contact) {
	QTcpSocket* socket = new QTcpSocket;
	socket->connectToHost(contact.getHost(), contact.getPort());
	setSocket(socket);
}

Connection::Connection(QObject* parent, QHostAddress host, quint16 port)
		: QObject(parent), m_socket(0), m_contact(0) {
	QTcpSocket* socket = new QTcpSocket;
	socket->connectToHost(host, port);
	setSocket(socket);
}

void Connection::setSocket(QTcpSocket* socket) {
	this->m_socket = socket;
	QObject::connect(socket, &QTcpSocket::readyRead, this, &Connection::onReadyRead);
	QObject::connect(socket, &QTcpSocket::connected, this, &Connection::onConnected);
	QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
	SLOT(onError(QAbstractSocket::SocketError)));
}

void Connection::onReadyRead() {
	emit readyRead();
}

void Connection::onConnected() {
	emit connected();
}

void Connection::onError(QAbstractSocket::SocketError error) {
	emit error(error);
	qDebug() << "Connection::onError()" << error;
}
