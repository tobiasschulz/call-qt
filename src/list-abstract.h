/*
 * list-abstract.h
 *
 *  Created on: 10.10.2013
 *      Author: tobias
 */

#ifndef LIST_ABSTRACT_H_
#define LIST_ABSTRACT_H_

#include <QObject>

#include "contact.h"

namespace List
{
	class Abstract: public QObject, public ID
	{
	Q_OBJECT
	public:
		explicit Abstract(QObject* parent);

	signals:
		void beginListReset(int oldcount, int newcount);
		void endListReset();
		void itemChanged(int i);
	};
}

#endif /* LIST_ABSTRACT_H_ */
