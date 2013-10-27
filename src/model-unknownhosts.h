#ifndef MODEL_UNKNOWNHOSTS_H
#define MODEL_UNKNOWNHOSTS_H

#include "contact.h"
#include "model-abstract.h"

class ContactList;
class ContactScanner;

namespace Model
{
	class UnknownHosts: public Abstract
	{
	Q_OBJECT
	public:
		explicit UnknownHosts(Abstract* parentmodel, QObject* parent = 0);
		QString id() const;

		int internalSize() const;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		Contact getContact(const QModelIndex& index) const;
		User getUser(const QModelIndex& index) const;

	private:
	};
}

#endif // MODEL_UNKNOWNHOSTS_H
