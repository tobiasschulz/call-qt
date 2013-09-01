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
#include <QThread>

#include "id.h"
#include "systemutil.h"
#include "config.h"

#include <iostream>
using namespace std;

#define PADDING_THREADNAME 12
#define PADDING_ID 35

QString fill(const QString& str, int size)
{
	return str + QString().fill(' ', (str.size() < size ? size - str.size() : 0));
}

QString threadname()
{
	const QString& objectname = QThread::currentThread()->objectName();
	return objectname.size() > 0 ? objectname : "main";
}

Log::Log(ID* id)
		: m_id(id)
{
}

QString Log::print(QTcpSocket* socket)
{
	return (QString("%1:%2").arg((socket)->localAddress().toString(), QString::number((socket)->localPort())));
}
QString Log::print(ID* id)
{
	return id->id();
}
QString Log::print(const ID& id)
{
	return id.id();
}
QString Log::print(QAudioDeviceInfo device)
{
	return device.deviceName();
}
QString Log::print(QAudioFormat format)
{
	return QString("QAudioFormat<freq=%1,channels=%2,samplesize=%3>").arg(QString::number(format.sampleRate()),
			QString::number(format.channelCount()), QString::number(format.sampleSize()));
}

void Log::debug(QString format) const
{
	QString thread(fill("(" + threadname() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format);
	qDebug("%s %s %s", Q(thread), Q(classid), Q(params));
}
void Log::debug(QString format, QVariant arg1) const
{
	QString thread(fill("(" + threadname() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString()));
	qDebug("%s %s %s", Q(thread), Q(classid), Q(params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2) const
{
	QString thread(fill("(" + threadname() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString()));
	qDebug("%s %s %s", Q(thread), Q(classid), Q(params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3) const
{
	QString thread(fill("(" + threadname() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString()));
	qDebug("%s %s %s", Q(thread), Q(classid), Q(params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) const
{
	QString thread(fill("(" + threadname() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString()));
	qDebug("%s %s %s", Q(thread), Q(classid), Q(params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5) const
{
	QString thread(fill("(" + threadname() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString()));
	qDebug("%s %s %s", Q(thread), Q(classid), Q(params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6) const
{
	QString thread(fill("(" + threadname() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString()));
	qDebug("%s %s %s", Q(thread), Q(classid), Q(params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6, QVariant arg7) const
{
	QString thread(fill("(" + threadname() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString()));
	qDebug("%s %s %s", Q(thread), Q(classid), Q(params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6, QVariant arg7, QVariant arg8) const
{
	QString thread(fill("(" + threadname() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString(), arg8.toString()));
	qDebug("%s %s %s", Q(thread), Q(classid), Q(params));
}

ID::ID()
		: log(this)
{
}
QString ID::id() const
{
	return "This should be implemented!";
}
QString ID::serialize() const
{
	return id();
}
QString ID::print(PrintFormat format) const
{
	return id();
}
const Log& ID::logger() const
{
	return log;
}

StaticID::StaticID(QString id)
		: m_id(id)
{
}
QString StaticID::id() const
{
	return m_id;
}

InvalidID::InvalidID()
{
}
QString InvalidID::id() const
{
	return "invalid";
}

uint qHash(const ID& c)
{
	return qHash(c.id());
}
