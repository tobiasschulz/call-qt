/*
 * networkutil.cpp
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#include <QHostInfo>
#include <QProcessEnvironment>
#include <QFile>
#include <QTime>
#include <QDate>
#include <QMutex>

#include "networkutil.h"
#include "systemutil.h"
#include "config.h"

// setsockopt -- has to be after Qt includes for Q_OS_WIN definition
#if defined(Q_OS_WIN)
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <unistd.h> // for socket code on Q_OS_WIN
#else
#include <sys/socket.h>
#include <unistd.h>
#endif

const Log NetworkUtil::log(new StaticId("NetworkUtil"));
NetworkUtil* NetworkUtil::m_instance;

NetworkUtil::NetworkUtil(QObject *parent)
		: QObject(parent)
{
}

NetworkUtil* NetworkUtil::instance()
{
	static QMutex mutex;
	if (!m_instance) {
		mutex.lock();
		if (!m_instance)
			m_instance = new NetworkUtil;
		mutex.unlock();
	}
	return m_instance;
}

QHostAddress NetworkUtil::parseHostname(QString hostname)
{
	QHostAddress hostaddr(hostname);

	if (hostaddr.protocol() == QAbstractSocket::IPv4Protocol
			|| hostaddr.protocol() == QAbstractSocket::IPv6Protocol) {
		log.debug("Valid IPv4 address.");

	} else {
		log.debug("Unknown or invalid address.");
		QHostInfo info = QHostInfo::fromName(hostname);
		if (!info.addresses().isEmpty()) {
			hostaddr = info.addresses().first();
		}
	}

	return hostaddr;
}

QString NetworkUtil::parseAddress(QHostAddress address)
{
	QHostInfo info = QHostInfo::fromName(address.toString());
	return info.hostName();
}

void NetworkUtil::writeHeaders(QTcpSocket* socket, Connection::Type type)
{
	writeLine(socket, "User: ", SystemUtil::instance()->getUserName());
	writeLine(socket, "UID: ", QVariant::fromValue(Config::uid()));
	writeLine(socket, "Uptime: ", QVariant::fromValue(Config::uptime()));

	if (type == Connection::STATUS)
		writeLine(socket, "Request: Status");
	else if (type == Connection::CALL)
		writeLine(socket, "Request: Call");
	else if (type == Connection::CHAT)
		writeLine(socket, "Request: Chat");
	else if (type == Connection::PING)
		writeLine(socket, "Request: Ping");
	else if (type == Connection::SERVER)
		writeLine(socket, "Request: ServerRole");

	writeLine(socket, "");
	socket->flush();

}

QHash<QString, QString>* NetworkUtil::readHeaders(QTcpSocket* socket)
{
	QHash<QString, QString>* headers = new QHash<QString, QString>;

	while (socket->isOpen() && socket->waitForReadyRead(Config::SOCKET_READ_TIMEOUT)) {
		while (socket->canReadLine()) {
			QString line = readLine(socket).trimmed();
			int index = line.indexOf(":");

			if (index != -1 && index + 1 < line.size()) {
				QString key = line.left(index).trimmed().toLower();
				QString value = line.mid(index + 1).trimmed();
				(*headers)[key] = value;
				log.debug("Header: %1=%2", key, value);

			} else if (line.isEmpty()) {
				return headers;

			} else {
				log.debug("Invalid header: '%1'", line);
			}
		}
	}

	return headers;
}

bool NetworkUtil::writeLine(QTcpSocket* socket, QVariant line)
{
	socket->write(line.toString().toLocal8Bit());
	socket->write("\n");
	socket->flush();
	return socket->waitForBytesWritten();
}

bool NetworkUtil::writeLine(QTcpSocket* socket, QVariant str1, QVariant str2)
{
	return writeLine(socket, str1.toString() + str2.toString());
}

bool NetworkUtil::writeLine(QTcpSocket* socket, QVariant str1, QVariant str2, QVariant str3)
{
	return writeLine(socket, str1.toString() + str2.toString() + str3.toString());
}

QString NetworkUtil::readLine(QTcpSocket* socket)
{
	char buf[32 * 1024];
	qint64 lineLength = socket->readLine(buf, sizeof(buf));
	if (lineLength != -1) {
		return QString::fromLocal8Bit(buf, lineLength);
	} else {
		return QString();
	}
}

void NetworkUtil::setSocketReuseAddr(QTcpSocket* socket)
{
	int reuse_addr_val = 1;
#if defined(Q_OS_WIN)
	int ret = setsockopt(m_tcpSocket->socketDescriptor(), SOL_SOCKET,
			SO_REUSEADDR, (char*) &reuse_addr_val,
			sizeof(reuse_addr_val));
#else
	int ret = setsockopt(socket->socketDescriptor(), SOL_SOCKET,
	SO_REUSEADDR, &reuse_addr_val, sizeof(reuse_addr_val));
#endif
	if (ret < 0) {
		log.debug("Failed to set SO_REUSEADDR");
	}
}

void NetworkUtil::setSocketTimeout(QTcpSocket* socket, int sec)
{
#if defined(Q_OS_WIN)
	unsigned int timeout = sec * 1000;
	if (-1 == setsockopt(socket->socketDescriptor(), SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout))) {
		log.debug("Failed: set SO_RCVTIMEO to %1 (windows)", sec);
	} else {
		log.debug("Success: set SO_RCVTIMEO to %1 (windows)", sec);
	}
	if (-1 == setsockopt(socket->socketDescriptor(), SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout))) {
		log.debug("Failed: set SO_SNDTIMEO to %1 (windows)", sec);
	} else {
		log.debug("Success: set SO_SNDTIMEO to %1 (windows)", sec);
	}
#else
	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = 0;

	if (setsockopt(socket->socketDescriptor(), SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout))
			< 0) {
		log.debug("Failed: set SO_RCVTIMEO to %1 (linux)", sec);
	} else {
		log.debug("Success: set SO_RCVTIMEO to %1 (linux)", sec);
	}
	if (setsockopt(socket->socketDescriptor(), SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout))
			< 0) {
		log.debug("Failed: set SO_SNDTIMEO to %1 (linux)", sec);
	} else {
		log.debug("Success: set SO_SNDTIMEO to %1 (linux)", sec);
	}
#endif
}

void NetworkUtil::setStandardSocketOptions(QTcpSocket* socket)
{
	socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
	NetworkUtil::setSocketReuseAddr(socket);
	NetworkUtil::setSocketTimeout(socket, Config::SOCKET_READ_TIMEOUT);
}

