#include <QHostInfo>

#include "connection.h"
#include "config.h"
#include "networkutil.h"

Connection::Connection(Type type, QObject* parent)
		: QObject(parent), m_socket(0), m_host(), m_contact(Contact::INVALID_CONTACT), m_headers(0), m_type(type), m_description(
				"?"), m_connecttimer(this), m_readtimer(this) {
}
QString Connection::id() const {
	return "Connection<" + m_description + ">";
}

void Connection::connect(QTcpSocket* socket) {
	m_host = Host(socket->peerAddress(), socket->peerPort());
	m_description = Log::print(socket);
	log.debug("connect(QTcpSocket*)");

	// set socket
	setSocket(socket);
}
void Connection::connect(Host host) {
	m_host = host;
	m_description = Log::print(host);
	log.debug("connect(QHostAddress, quint16)");

	// connect
	setSocket(new QTcpSocket(this));
	m_socket->abort();
	m_socket->connectToHost(host.address(), host.port());

	// start connect timer
	QObject::connect(&m_connecttimer, &QTimer::timeout, this, &Connection::onConnectTimeout);
	m_connecttimer.setSingleShot(true);
	m_connecttimer.start(Config::SOCKET_CONNECT_TIMEOUT);
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
	emit connectFailed("Connect Timeout", m_host);
	emit hostOffline(m_host);
}
void Connection::onReadTimeout() {
	m_socket->abort();
	m_socket->close();
	log.debug("socketError(Read Timeout)");
	emit socketError("Read Timeout", m_host);
}

bool Connection::isConnected() {
	return m_socket->state() == QAbstractSocket::ConnectedState;
}

void Connection::onReadyRead() {
	if (m_readtimer.isActive()) {
		m_readtimer.start(Config::SOCKET_READ_TIMEOUT);
	}
	log.debug("onReadyRead()");
	emit readyRead();
}

void Connection::onConnected() {
	m_connecttimer.stop();
	log.debug("onConnected()");

	NetworkUtil::instance()->setStandardSocketOptions(m_socket);

	QObject::connect(m_socket, &QTcpSocket::readyRead, this, &Connection::onReadyRead);
	QObject::connect(&m_readtimer, &QTimer::timeout, this, &Connection::onReadTimeout);
	m_readtimer.start(Config::SOCKET_READ_TIMEOUT);

	NetworkUtil::instance()->writeHeaders(m_socket, m_type);
	m_headers = NetworkUtil::instance()->readHeaders(m_socket);

	QHostAddress host = m_socket->peerAddress();
	quint16 port = m_socket->peerPort();
	QString user = m_headers->value("user", Contact::INVALID_USER);

	m_contact = Contact(user, host, port);
	emit contactFound(m_contact);

	emit connected();
	emit hostOnline(m_host);
}

void Connection::onDisconnected() {
	m_readtimer.stop();
	log.debug("onDisconnected()");
	emit disconnected();
}

void Connection::onError(QAbstractSocket::SocketError err) {
	QString errString = m_socket->errorString();
	log.debug("Connection::onError(%1)", errString);
	m_socket->abort();
	m_socket->close();
	//emit error(err);
	if (m_connecttimer.isActive()) {
		m_connecttimer.stop();
		emit connectFailed("Socket Error: " + errString, m_host);
	} else if (m_readtimer.isActive()) {
		m_readtimer.stop();
		emit socketError("Socket Error: " + errString, m_host);
	}
}

