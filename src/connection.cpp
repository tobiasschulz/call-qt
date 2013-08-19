#include <QHostInfo>

#include "connection.h"
#include "config.h"
#include "util.h"

Connection::Connection(QTcpSocket* socket, Type type, QObject* parent)
		: QObject(parent), m_socket(0), m_contact(0), m_headers(0), m_type(type), m_description() {
	m_description = QString("socket=%1").arg(Log::print(socket));
}
Connection::Connection(Contact* contact, Type type, QObject* parent)
		: QObject(parent), m_socket(0), m_contact(contact), m_headers(0), m_type(type), m_description() {
	m_description = QString("contact=%1").arg(contact->toString());
}
Connection::Connection(QHostAddress hostaddr, quint16 port, Type type, QObject* parent)
		: QObject(parent), m_socket(0), m_contact(0), m_headers(0), m_type(type), m_description() {
	m_description = QString("hostaddr=%1,port=%2").arg(hostaddr.toString(), QString::number(port));
}

void Connection::connect(QTcpSocket* socket) {
	log.debug("connect");
	setSocket(socket);
}
void Connection::connect(Contact* contact) {
	log.debug("connect");
	setSocket(new QTcpSocket(this));
	m_socket->abort();
	m_socket->connectToHost(contact->getHost(), contact->getPort());
}
void Connection::connect(QHostAddress hostaddr, quint16 port) {
	log.debug("connect");
	setSocket(new QTcpSocket(this));
	m_socket->abort();
	m_socket->connectToHost(hostaddr, port);
}

void Connection::setSocket(QTcpSocket* socket) {
	this->m_socket = socket;

	QObject::connect(m_socket, &QTcpSocket::disconnected, this, &Connection::onDisconnected);
	QObject::connect(m_socket, &QTcpSocket::connected, this, &Connection::onConnected);
	QObject::connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
	SLOT(onError(QAbstractSocket::SocketError)));

	log.debug("socket=%1, socket->state()=%2", Log::print(m_socket), m_socket->state());
	if (m_socket->state() == QAbstractSocket::ConnectedState) {
		emit m_socket->connected();
		//onConnected();
	}
}

bool Connection::isConnected() {
	return m_socket->state() == QAbstractSocket::ConnectedState;
}

void Connection::onReadyRead() {
	log.debug("onReadyRead()");
	emit readyRead();
}

void Connection::onConnected() {
	log.debug("onConnected()");
	m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
	NetworkUtil::setSocketReuseAddr(m_socket);
	NetworkUtil::setSocketTimeout(m_socket, Config::SOCKET_READ_TIMEOUT);

	log.debug("1");
	NetworkUtil::writeHeaders(m_socket, m_type);
	log.debug("2");
	m_headers = NetworkUtil::readHeaders(m_socket);
	log.debug("3");

// QAbstractSocket::ConnectedState

	QObject::connect(m_socket, &QTcpSocket::readyRead, this, &Connection::onReadyRead);
	emit connected();
}

void Connection::onDisconnected() {
	log.debug("onDisconnected()");
	emit disconnected();
}

void Connection::onError(QAbstractSocket::SocketError err) {
	log.debug("Connection::onError(%1)", m_socket->errorString());
	if (err == QAbstractSocket::SocketTimeoutError) {
		m_socket->abort();
		m_socket->close();
	}
	emit error(err);
}
