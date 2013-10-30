/*
 * util.cpp
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
#include <QMutexLocker>

#include <iostream>
using namespace std;

#include "systemutil.h"
#include "config.h"

#if defined(Q_OS_WIN)
#  include <windows.h>
#  include <lm.h>
#  undef ERROR
#else
#  include <sys/types.h>
#  include <pwd.h>
#  include <errno.h>
#  include <stdio.h>
#  include <string.h>
#endif

const Log SystemUtil::log(new StaticID("SystemUtil"));
SystemUtil* SystemUtil::m_instance;

SystemUtil::SystemUtil(QObject *parent)
		: QObject(parent)
{
	QObject::connect(this, &SystemUtil::newLogMessage, this, &SystemUtil::printLogMessageConsole);
	QObject::connect(this, &SystemUtil::newLogMessage, this, &SystemUtil::printLogMessageFile);
}

SystemUtil* SystemUtil::instance()
{
	static QMutex mutex;
	if (!m_instance) {
		mutex.lock();
		if (!m_instance)
			m_instance = new SystemUtil;
		mutex.unlock();
	}
	return m_instance;
}

QString SystemUtil::getUserName()
{
	static QString username;
	if (username.isEmpty()) {
		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		if (env.contains("USER")) {
			username = env.value("USER");
		} else if (env.contains("USERNAME")) {
			username = env.value("USERNAME");
		} else {
			username = "user";
		}
	}
	return username;
}

QString SystemUtil::getUserFullName()
{
	QString username = getUserName();
	static QString fullname;
	if (fullname.isEmpty()) {
#if defined(Q_OS_WIN)
		LPUSER_INFO_23 buffer = NULL;
		NET_API_STATUS nStatus = NetUserGetInfo(NULL, username.toStdWString().c_str(), 23, (LPBYTE *) &buffer);

		if (nStatus != NERR_Success) {
			log.error("getUserFullName(): nStatus=%1 (!= NERR_Success=%2)", QString::number(nStatus),
					QString::number(NERR_Success));
			return username;
		} else if (buffer == NULL) {
			log.error("getUserFullName(): pBuf=NULL");
			return username;
		} else {
			QString shortname(QString::fromWCharArray(buffer->usri23_name));
			fullname = QString::fromWCharArray(buffer->usri23_full_name);
			log.debug("getUserFullName(): Short name: %1", shortname);
		}
#else
		struct passwd* passwd = getpwnam(username.toLocal8Bit());
		QString gecos(QString::fromLocal8Bit(passwd->pw_gecos));
		fullname = gecos.split(',')[0];
		fullname = fullname.size() > 0 ? fullname : username;
#endif
	}
	log.debug("getUserFullName(): User account name: %1", username);
	log.debug("getUserFullName(): Full name: %1", fullname);
	return fullname;
}

QString SystemUtil::getComputerName()
{
	return QHostInfo::localHostName();
}

void SystemUtil::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString& msg)
{
	Q_UNUSED(context);
	ID::Verbosity verbosity = type == QtDebugMsg ? ID::DEBUG : type == QtWarningMsg ? ID::WARNING : ID::ERROR;
	QString str = createLogMessage(verbosity, msg);
	emit newLogMessage(verbosity, Log::currentThreadName(), str);
}

void SystemUtil::messageOutput(ID::Verbosity verbosity, const QString& msg)
{
	QString str = createLogMessage(verbosity, msg);
	emit newLogMessage(verbosity, Log::currentThreadName(), str);
}

void SystemUtil::printLogMessageConsole(ID::Verbosity type, QString thread, QString str)
{
	Q_UNUSED(type);
	Q_UNUSED(thread);
	std::cout << str.toLocal8Bit().constData() << std::flush;
}

void SystemUtil::printLogMessageFile(ID::Verbosity type, QString thread, QString str)
{
	Q_UNUSED(type);
	Q_UNUSED(thread);
#if defined(Q_OS_WIN)
	Q_UNUSED(str);
#else
	static QTextStream* out = 0;
	static bool initialized = false;
	if (!initialized) {
		initialized = true;
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		QFile file("debug.log");
		if (file.open(QIODevice::Append | QIODevice::Text) && (file.openMode() & QIODevice::WriteOnly)) {
			out = new QTextStream(&file);
		}
	}
	if (initialized && out != 0) {
		*out << str;
		out->flush();
	}
#endif
}

QString SystemUtil::createLogMessage(ID::Verbosity type, const QString &msg)
{
	QString str;
	str += QDate::currentDate().toString("yyyy-MM-dd ");
	str += QTime::currentTime().toString("hh:mm:ss ");

	switch (type) {
	case ID::DEBUG:
		str += "(debug)";
		break;
	case ID::INFO:
		str += "(info) ";
		break;
	case ID::WARNING:
		str += "(warn) ";
		break;
	case ID::ERROR:
		str += "(error)";
		break;
	case ID::ALL:
	case ID::NONE:
		str += "(-----)";
		break;
	}

	str += "  " + msg + "\n";
	return str;
}

