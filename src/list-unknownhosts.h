/*
 * list-unknownhosts.h
 *
 *  Created on: 10.10.2013
 *      Author: tobias
 */

#ifndef LIST_UNKNOWNHOSTS_H_
#define LIST_UNKNOWNHOSTS_H_

#include <QList>
#include <QStringList>
#include <QMutex>
#include <QMutexLocker>

#include "contact.h"
#include "list-abstract.h"

namespace List
{
	class UnknownHosts: public Abstract
	{
	Q_OBJECT
	public:
		static UnknownHosts& instance();
		QString id() const;

		const QString& get(int index) const;
		int size() const;
		QList<QString> toList() const;

	public slots:
		void onHostStateChanged(Host host);

	private slots:
		void rebuildItems();

	private:
		explicit UnknownHosts(QObject* parent = 0);

		QStringList m_unknownhosts;
		static QMutex m_lock;

		friend class ContactList;
	};
}

#endif /* LIST_UNKNOWNHOSTS_H_ */
