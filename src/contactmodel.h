#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include <QObject>
#include <QPointer>
#include <QAbstractItemModel>

#include "contact.h"
#include "model-abstract.h"

class ContactList;
class ContactScanner;

class ContactModel: public Model::Abstract
{
Q_OBJECT
public:
	explicit ContactModel(QObject* parent = 0);
	QString id() const;

	int size() const;
	virtual int offset(Abstract* submodel) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	const Contact& getContact(const QModelIndex& index) const;

private:
	QList<Model::Abstract*> models;
};

#endif // CONTACTMODEL_H
