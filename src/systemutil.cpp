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
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	if (env.contains("USER")) {
		return env.value("USER");
	} else if (env.contains("USERNAME")) {
		return env.value("USERNAME");
	} else {
		return "user";
	}
}

void SystemUtil::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString& msg)
{
	QString str = createLogMessage(type == QtDebugMsg ? ID::DEBUG : type == QtWarningMsg ? ID::WARN : ID::ERROR, msg);
	emit newLogMessage(str);
}
void SystemUtil::messageOutput(ID::Verbosity type, const QString& msg)
{
	QString str = createLogMessage(type, msg);
	emit newLogMessage(str);
}
void SystemUtil::printLogMessageConsole(QString str)
{
	std::cout << str.toLocal8Bit().constData() << std::flush;

}
void SystemUtil::printLogMessageFile(QString str)
{
#if !defined(Q_OS_WIN)
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
	case ID::WARN:
		str += "(warn) ";
		break;
	case ID::ERROR:
		str += "(error)";
		break;
	}

	str += "  " + msg + "\n";
	return str;
}

