#ifndef MODEL_ABSTRACT_H
#define MODEL_ABSTRACT_H

#include <QObject>
#include <QPointer>
#include <QAbstractItemModel>

#include "contact.h"

namespace Model
{
	class Abstract: public QAbstractItemModel, public ID
	{
	Q_OBJECT
	public:
		explicit Abstract(Abstract* parentmodel, QObject* parent = 0);

		virtual QString id() const = 0;
		virtual int size() const = 0;
		virtual int offset(Abstract* submodel) const;
		virtual const Contact& getContact(const QModelIndex& index) const = 0;

		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex& child) const;
		bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole);

	public slots:
		void beginSetItems(int oldcount, int newcount);
		void endSetItems();
		void onStateChanged(int i);

		void onShowConnections(bool show);

	private:
		void beginInsertItems(int start, int end);
		void endInsertItems();
		void beginRemoveItems(int start, int end);
		void endRemoveItems();
		void changeItems(int start, int end);

	protected:
		QPointer<Abstract> parentmodel;
		bool showConnections;
	};
}

#endif // MODEL_ABSTRACT_H
