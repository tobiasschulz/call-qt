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

class NetworkUtil: public QObject
{
Q_OBJECT

public:
	static NetworkUtil* instance();

	QHostAddress parseHostname(QString);
	QString parseAddress(QHostAddress);
	void writeHeaders(QTcpSocket* socket, Connection::Type type, const Log* logger = &log);
	QHash<QString, QString> readHeaders(QTcpSocket* socket, const Log* logger = &log);
	QAudioFormat readAudioFormat(QHash<QString, QString> headers);
	bool writeLine(QTcpSocket* socket, QVariant str1);
	bool writeLine(QTcpSocket* socket, QVariant str1, QVariant str2);
	bool writeLine(QTcpSocket* socket, QVariant str1, QVariant str2, QVariant str3);
	QString readLine(QTcpSocket* socket);
	void setSocketReuseAddr(QTcpSocket* socket);
	void setSocketTimeout(QTcpSocket* socket, int timeout);
	void setStandardSocketOptions(QTcpSocket* socket);

private:
	explicit NetworkUtil(QObject *parent = 0);
	NetworkUtil(const NetworkUtil &); // hide copy constructor
	NetworkUtil& operator=(const NetworkUtil &); // hide assign op
	// we leave just the declarations, so the compiler will warn us
	// if we try to use those two functions by accident
	static NetworkUtil* m_instance;

	static const Log log;
};

#endif /* NETWORKUTIL_H */
