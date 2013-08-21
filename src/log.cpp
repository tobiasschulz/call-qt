/*
 * log.cpp
 *
 *  Created on: 19.08.2013
 *      Author: tobias
 */

#include <QFile>
#include <QTime>
#include <QDate>
#include <QHostAddress>

#include "log.h"
#include "systemutil.h"
#include "config.h"

#include <iostream>
using namespace std;

Log::Log(Id* id)
		: m_id(id) {
}

QString Log::print(QTcpSocket* socket) {
	return (QString("%1:%2").arg((socket)->localAddress().toString(), QString::number((socket)->localPort())));
}
QString Log::print(Id* id) {
	return id->id();
}
QString Log::print(const Id& id) {
	return id.id();
}

void Log::debug(QString format) const {
	qDebug("[%s]: %s", Q(m_id->id()), Q(format));
}
void Log::debug(QString format, QVariant arg1) const {
	qDebug("[%s]: %s", Q(m_id->id()), Q(format.arg(arg1.toString())));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2) const {
	qDebug("[%s]: %s", Q(m_id->id()), Q(format.arg(arg1.toString(), arg2.toString())));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3) const {
	qDebug("[%s]: %s", Q(m_id->id()), Q(format.arg(arg1.toString(), arg2.toString(), arg3.toString())));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) const {
	qDebug(
	"[%s]: %s", Q(m_id->id()), Q(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString())));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5) const {
	qDebug(
			"[%s]: %s", Q(m_id->id()), Q(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString())));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6) const {
	qDebug(
			"[%s]: %s", Q(m_id->id()), Q(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(), arg6.toString())));
}

Id::Id()
		: log(this) {
}
QString Id::id() const {
	return "This should be implemented!";
}

StaticId::StaticId(QString id)
		: m_id(id) {
}
QString StaticId::id() const {
	return m_id;
}

