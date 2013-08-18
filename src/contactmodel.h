#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include <QObject>
#include <QAbstractItemModel>

class ContactList;
class ContactScanner;

class ContactModel: public QAbstractItemModel {
Q_OBJECT
public:
	explicit ContactModel(QObject* parent = 0);

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& child) const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole);

signals:

public slots:
	void beginInsertItems(int start, int end);
	void endInsertItems();
	void beginRemoveItems(int start, int end);
	void endRemoveItems();
	void changeItems(int start, int end);

private:
	ContactList* m_contacts;
	ContactScanner* m_scanner;

};

#endif // CONTACTMODEL_H
