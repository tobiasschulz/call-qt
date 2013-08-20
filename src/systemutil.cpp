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

#include <iostream>
using namespace std;

#include "systemutil.h"
#include "config.h"

const Log SystemUtil::log(new StaticId("SystemUtil"));
SystemUtil* SystemUtil::m_instance;

SystemUtil::SystemUtil(QObject *parent)
		: QObject(parent) {
}

SystemUtil* SystemUtil::instance() {
	static QMutex mutex;
	if (!m_instance) {
		mutex.lock();
		if (!m_instance)
			m_instance = new SystemUtil;
		mutex.unlock();
	}
	return m_instance;
}

QString SystemUtil::getUserName() {
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	if (env.contains("USER")) {
		return env.value("USER");
	} else if (env.contains("USERNAME")) {
		return env.value("USERNAME");
	} else {
		return "user";
	}
}

void SystemUtil::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	QString str = createLogMessage(type, context, msg);

	std::cout << str.toLocal8Bit().constData();

	QFile file("debug.log");
	file.open(QIODevice::Append | QIODevice::Text);
	QTextStream out(&file);
	out << str;
	out.flush();

	emit newLogMessage(str);
}

QString SystemUtil::createLogMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	QString str;
	str += QDate::currentDate().toString("yyyy-MM-dd ");
	str += QTime::currentTime().toString("hh:mm:ss ");

	switch (type) {
	case QtDebugMsg:
		str += "(debug)";
		break;
	case QtWarningMsg:
		str += "(warn) ";
		break;
	case QtCriticalMsg:
		str += "(crit) ";
		break;
	case QtFatalMsg:
		str += "(fatal)";
		break;
	}

	str += "  " + msg + "\n";
	return str;
}

