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

#define LOG_DEBUG(str) SystemUtil::instance()->messageOutput(ID::DEBUG, str)
#define LOG_INFO(str) SystemUtil::instance()->messageOutput(ID::INFO, str)
#define LOG_WARN(str) SystemUtil::instance()->messageOutput(ID::WARNING, str)
#define LOG_ERROR(str) SystemUtil::instance()->messageOutput(ID::ERROR, str)

QString fill(const QString& str, int size)
{
	return str + QString().fill(' ', (str.size() < size ? size - str.size() : 0));
}

QString Log::currentThreadName()
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

ID::ID()
		: log(this), m_verbose(ALL)
{
}
ID::~ID()
{
	foreach (QConnection* connection, m_connections)
	{
		QObject::disconnect(*connection);
		free(connection);
	}
}
ID& operator<<(ID& obj, ID::QConnection connection)
{
	obj.m_connections << new ID::QConnection(connection);
	return obj;
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
	Q_UNUSED(format);
	return id();
}
const Log& ID::logger() const
{
	return log;
}

void ID::setVerbose(ID::Verbosity verbose)
{
	m_verbose = verbose;
}
ID::Verbosity ID::verbose() const
{
	return m_verbose;
}
bool ID::isVerbose(ID::Verbosity verbose) const
{
	switch (verbose) {
	case ALL:
	case DEBUG:
		return m_verbose == ALL || m_verbose == DEBUG;
	case INFO:
		return m_verbose == ALL || m_verbose == DEBUG || m_verbose == INFO;
	case WARNING:
		return m_verbose == ALL || m_verbose == DEBUG || m_verbose == INFO || m_verbose == WARNING;
	case ERROR:
		return m_verbose == ALL || m_verbose == DEBUG || m_verbose == INFO || m_verbose == WARNING || m_verbose == ERROR;
	case NONE:
		return true;
	}
	return true;
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

void Log::debug(QString format) const
{
	if (!m_id->isVerbose(ID::DEBUG))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format);
	LOG_DEBUG(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::debug(QString format, QVariant arg1) const
{
	if (!m_id->isVerbose(ID::DEBUG))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString()));
	LOG_DEBUG(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2) const
{
	if (!m_id->isVerbose(ID::DEBUG))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString()));
	LOG_DEBUG(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3) const
{
	if (!m_id->isVerbose(ID::DEBUG))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString()));
	LOG_DEBUG(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) const
{
	if (!m_id->isVerbose(ID::DEBUG))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString()));
	LOG_DEBUG(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5) const
{
	if (!m_id->isVerbose(ID::DEBUG))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString()));
	LOG_DEBUG(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6) const
{
	if (!m_id->isVerbose(ID::DEBUG))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString()));
	LOG_DEBUG(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6, QVariant arg7) const
{
	if (!m_id->isVerbose(ID::DEBUG))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString()));
	LOG_DEBUG(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6, QVariant arg7, QVariant arg8) const
{
	if (!m_id->isVerbose(ID::DEBUG))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString(), arg8.toString()));
	LOG_DEBUG(QString("%1 %2 %3").arg(thread, classid, params));
}

void Log::info(QString format) const
{
	if (!m_id->isVerbose(ID::INFO))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format);
	LOG_INFO(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::info(QString format, QVariant arg1) const
{
	if (!m_id->isVerbose(ID::INFO))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString()));
	LOG_INFO(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::info(QString format, QVariant arg1, QVariant arg2) const
{
	if (!m_id->isVerbose(ID::INFO))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString()));
	LOG_INFO(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::info(QString format, QVariant arg1, QVariant arg2, QVariant arg3) const
{
	if (!m_id->isVerbose(ID::INFO))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString()));
	LOG_INFO(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::info(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) const
{
	if (!m_id->isVerbose(ID::INFO))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString()));
	LOG_INFO(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::info(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5) const
{
	if (!m_id->isVerbose(ID::INFO))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString()));
	LOG_INFO(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::info(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6) const
{
	if (!m_id->isVerbose(ID::INFO))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString()));
	LOG_INFO(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::info(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5, QVariant arg6,
		QVariant arg7) const
{
	if (!m_id->isVerbose(ID::INFO))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString()));
	LOG_INFO(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::info(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5, QVariant arg6,
		QVariant arg7, QVariant arg8) const
{
	if (!m_id->isVerbose(ID::INFO))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString(), arg8.toString()));
	LOG_INFO(QString("%1 %2 %3").arg(thread, classid, params));
}

void Log::warn(QString format) const
{
	if (!m_id->isVerbose(ID::WARNING))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format);
	LOG_WARN(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::warn(QString format, QVariant arg1) const
{
	if (!m_id->isVerbose(ID::WARNING))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString()));
	LOG_WARN(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::warn(QString format, QVariant arg1, QVariant arg2) const
{
	if (!m_id->isVerbose(ID::WARNING))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString()));
	LOG_WARN(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::warn(QString format, QVariant arg1, QVariant arg2, QVariant arg3) const
{
	if (!m_id->isVerbose(ID::WARNING))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString()));
	LOG_WARN(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::warn(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) const
{
	if (!m_id->isVerbose(ID::WARNING))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString()));
	LOG_WARN(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::warn(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5) const
{
	if (!m_id->isVerbose(ID::WARNING))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString()));
	LOG_WARN(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::warn(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6) const
{
	if (!m_id->isVerbose(ID::WARNING))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString()));
	LOG_WARN(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::warn(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5, QVariant arg6,
		QVariant arg7) const
{
	if (!m_id->isVerbose(ID::WARNING))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString()));
	LOG_WARN(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::warn(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5, QVariant arg6,
		QVariant arg7, QVariant arg8) const
{
	if (!m_id->isVerbose(ID::WARNING))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString(), arg8.toString()));
	LOG_WARN(QString("%1 %2 %3").arg(thread, classid, params));
}

void Log::error(QString format) const
{
	if (!m_id->isVerbose(ID::ERROR))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format);
	LOG_ERROR(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::error(QString format, QVariant arg1) const
{
	if (!m_id->isVerbose(ID::ERROR))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString()));
	LOG_ERROR(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::error(QString format, QVariant arg1, QVariant arg2) const
{
	if (!m_id->isVerbose(ID::ERROR))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString()));
	LOG_ERROR(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::error(QString format, QVariant arg1, QVariant arg2, QVariant arg3) const
{
	if (!m_id->isVerbose(ID::ERROR))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString()));
	LOG_ERROR(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::error(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) const
{
	if (!m_id->isVerbose(ID::ERROR))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString()));
	LOG_ERROR(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::error(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5) const
{
	if (!m_id->isVerbose(ID::ERROR))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString()));
	LOG_ERROR(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::error(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6) const
{
	if (!m_id->isVerbose(ID::ERROR))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString()));
	LOG_ERROR(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::error(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6, QVariant arg7) const
{
	if (!m_id->isVerbose(ID::ERROR))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString()));
	LOG_ERROR(QString("%1 %2 %3").arg(thread, classid, params));
}
void Log::error(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
		QVariant arg6, QVariant arg7, QVariant arg8) const
{
	if (!m_id->isVerbose(ID::ERROR))
		return;
	QString thread(fill("(" + currentThreadName() + ")", PADDING_THREADNAME));
	QString classid(fill("[" + m_id->print() + "]:", PADDING_ID));
	QString params(
			format.arg(arg1.toString(), arg2.toString(), arg3.toString(), arg4.toString(), arg5.toString(),
					arg6.toString(), arg7.toString(), arg8.toString()));
	LOG_ERROR(QString("%1 %2 %3").arg(thread, classid, params));
}

