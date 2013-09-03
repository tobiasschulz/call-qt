/*
 * systemutil.h
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#ifndef SYSTEMUTIL_H
#define SYSTEMUTIL_H

#include <QObject>
#include <QString>
#include <QHash>

#include "id.h"

class SystemUtil: public QObject
{
Q_OBJECT

public:
	static SystemUtil* instance();

	QString getUserName();
	void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
	void messageOutput(ID::Verbosity type, const QString &msg);

signals:
	void newLogMessage(QString message);

public slots:
	void printLogMessageConsole(QString str);
	void printLogMessageFile(QString str);

private:
	explicit SystemUtil(QObject *parent = 0);
	SystemUtil(const SystemUtil &); // hide copy constructor
	SystemUtil& operator=(const SystemUtil &); // hide assign op
	// we leave just the declarations, so the compiler will warn us
	// if we try to use those two functions by accident
	static SystemUtil* m_instance;

	QString createLogMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
	QString createLogMessage(ID::Verbosity type, const QString &msg);

	static const Log log;
};

#endif /* SYSTEMUTIL_H */
