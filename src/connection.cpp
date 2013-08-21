#include <QHostInfo>

#include "connection.h"
#include "config.h"
#include "networkutil.h"

Connection::Connection(QTcpSocket* socket, Type type, QObject* parent)
		: QObject(parent), m_socket(0), m_contact(Contact::INVALID_CONTACT), m_headers(0), m_type(type), m_description(), connecttimer(
				this), readtimer(this) {
	m_description = Log::print(socket);
}
Connection::Connection(const Contact& contact, Type type, QObject* parent)
		: QObject(parent), m_socket(0), m_contact(contact), m_headers(0), m_type(type), m_description(), connecttimer(
				this), readtimer(this) {
	m_description = Log::print(contact);
}
Connection::Connection(QHostAddress hostaddr, quint16 port, Type type, QObject* parent)
		: QObject(parent), m_socket(0), m_contact(Contact::INVALID_CONTACT), m_headers(0), m_type(type), m_description(), connecttimer(
				this), readtimer(this) {
	m_description = QString("%1:%2").arg(hostaddr.toString(), QString::number(port));
}

void Connection::connect(QTcpSocket* socket) {
	log.debug("connect(QTcpSocket*)");

	// set socket
	setSocket(socket);
}
void Connection::connect(Contact contact) {
	connect(contact.host(), contact.port());
}
void Connection::connect(QHostAddress hostaddr, quint16 port) {
	log.debug("connect(QHostAddress, quint16)");

	// connect
	setSocket(new QTcpSocket(this));
	m_socket->abort();
	m_socket->connectToHost(hostaddr, port);

	// start connect timer
	QObject::connect(&connecttimer, &QTimer::timeout, this, &Connection::onConnectTimeout);
	connecttimer.setSingleShot(true);
	connecttimer.start(Config::SOCKET_CONNECT_TIMEOUT);
}

void Connection::setSocket(QTcpSocket* socket) {
	this->m_socket = socket;

	QObject::connect(this, &Connection::close, m_socket, &QTcpSocket::abort);
	QObject::connect(m_socket, &QTcpSocket::disconnected, this, &Connection::onDisconnected);
	QObject::connect(m_socket, &QTcpSocket::connected, this, &Connection::onConnected);
	QObject::connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
	SLOT(onError(QAbstractSocket::SocketError)));

	log.debug("socket=%1, socket->state()=%2", Log::print(m_socket), m_socket->state());
	if (m_socket->state() == QAbstractSocket::ConnectedState) {
		//onConnected();
		emit m_socket->connected();
	}
}

void Connection::onConnectTimeout() {
	m_socket->abort();
	m_socket->close();
	log.debug("connectFailed(Connect Timeout)");
	emit connectFailed("Connect Timeout");
}
void Connection::onReadTimeout() {
	m_socket->abort();
	m_socket->close();
	log.debug("socketError(Read Timeout)");
	emit socketError("Read Timeout");
}

bool Connection::isConnected() {
	return m_socket->state() == QAbstractSocket::ConnectedState;
}

void Connection::onReadyRead() {
	if (readtimer.isActive()) {
		readtimer.start(Config::SOCKET_READ_TIMEOUT);
	}
	log.debug("onReadyRead()");
	emit readyRead();
}

void Connection::onConnected() {
	connecttimer.stop();
	log.debug("onConnected()");

	NetworkUtil::setStandardSocketOptions(m_socket);

	QObject::connect(m_socket, &QTcpSocket::readyRead, this, &Connection::onReadyRead);
	QObject::connect(&readtimer, &QTimer::timeout, this, &Connection::onReadTimeout);
	readtimer.start(Config::SOCKET_READ_TIMEOUT);

	NetworkUtil::writeHeaders(m_socket, m_type);
	m_headers = NetworkUtil::readHeaders(m_socket);

	QHostAddress host = m_socket->peerAddress();
	quint16 port = m_socket->peerPort();
	QString user = m_headers->value("user", Contact::INVALID_USER);

	m_contact = Contact(user, host, port);
	emit contactFound(m_contact);

	emit connected();
}

void Connection::onDisconnected() {
	readtimer.stop();
	log.debug("onDisconnected()");
	emit disconnected();
}

void Connection::onError(QAbstractSocket::SocketError err) {
	QString errString = m_socket->errorString();
	log.debug("Connection::onError(%1)", errString);
	m_socket->abort();
	m_socket->close();
	//emit error(err);
	if (connecttimer.isActive()) {
		connecttimer.stop();
		emit connectFailed("Socket Error: " + errString);
	} else if (readtimer.isActive()) {
		readtimer.stop();
		emit socketError("Socket Error: " + errString);
	}
}

