/*
 * networkutil.h
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#ifndef NETWORKUTIL_H
#define NETWORKUTIL_H

#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>
#include <QHash>

#include "connection.h"

class NetworkUtil: public QObject {
Q_OBJECT

public:
	static QHostAddress parseHost(QString);
	static void writeHeaders(QTcpSocket* socket, Connection::Type type);
	static QHash<QString, QString>* readHeaders(QTcpSocket* socket);
	static bool writeLine(QTcpSocket* socket, QVariant str1);
	static bool writeLine(QTcpSocket* socket, QVariant str1, QVariant str2);
	static bool writeLine(QTcpSocket* socket, QVariant str1, QVariant str2, QVariant str3);
	static QString readLine(QTcpSocket* socket);
	static void setSocketReuseAddr(QTcpSocket* socket);
	static void setSocketTimeout(QTcpSocket* socket, int timeout);
	static void setStandardSocketOptions(QTcpSocket* socket);

private:
	static const Log log;
};

#endif /* NETWORKUTIL_H */
