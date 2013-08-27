/*
 * thread.cpp
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#include "thread.h"

Thread::Thread(QString name, QObject *parent)
		: QThread(parent)
{
	setObjectName(name);
	//log.debug("setObjectName(%1)", name);
}

QString Thread::id() const
{
	return "Thread<" + this->objectName() + ">";
}

void Thread::run()
{
	exec();
}

