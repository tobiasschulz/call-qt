/*
 * thread.h
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#ifndef THREAD_H
#define THREAD_H

#include <QThread>

#include "id.h"

class Thread: public QThread, public ID
{
Q_OBJECT

public:
	Thread(QString name = "NoNameThread", QObject *parent = 0);
	virtual QString id() const;

	void run();
};

#endif /* THREAD_H */
