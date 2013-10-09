#ifndef MODEL_CONTACTS_H
#define MODEL_CONTACTS_H

#include "contact.h"
#include "model-abstract.h"

class ContactList;
class ContactScanner;

namespace Model
{
	class Contacts: public Abstract
	{
	Q_OBJECT
	public:
		explicit Contacts(Abstract* parentmodel, QObject* parent = 0);
		QString id() const;

		int size() const;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		const Contact& getContact(const QModelIndex& index) const;
	};
}

#endif // MODEL_CONTACTS_H
