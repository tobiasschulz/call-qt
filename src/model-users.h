#ifndef MODEL_USERS_H
#define MODEL_USERS_H

#include "contact.h"
#include "model-abstract.h"

class ContactList;
class ContactScanner;

namespace Model
{
	class Users: public Abstract
	{
	Q_OBJECT
	public:
		explicit Users(Abstract* parentmodel, QObject* parent = 0);
		QString id() const;

		int size() const;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		Contact getContact(const QModelIndex& index) const;
		User getUser(const QModelIndex& index) const;
	};
}

#endif // MODEL_USERS_H
