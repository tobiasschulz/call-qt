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

		int size() const;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		const Contact& getContact(const QModelIndex& index) const;

	public slots:
		void onShowOfflineContacts(bool show);

	private:
		bool showOfflineContacts;
	};
}

#endif // MODEL_UNKNOWNHOSTS_H
