#include "model-abstract.h"

using namespace Model;

Abstract::Abstract(Abstract* parentmodel, QObject* parent)
		: QAbstractItemModel(parent), parentmodel(parentmodel)
{
}

int Abstract::offset(Abstract* submodel) const
{
	Q_UNUSED(submodel);
	return 0;
}

int Abstract::rowCount(const QModelIndex& parent) const
{
	return !parent.isValid() ? size() : 0;
}

int Abstract::columnCount(const QModelIndex& parent) const
{
	return !parent.isValid() ? 1 : 0;
}

QModelIndex Abstract::index(int row, int column, const QModelIndex& parent) const
{
	return !parent.isValid() && row >= 0 && row < rowCount(parent) && column >= 0 && column < columnCount(parent) ?
			createIndex(row, column) : QModelIndex();
}

QModelIndex Abstract::parent(const QModelIndex& child) const
{
	Q_UNUSED(child);
	return QModelIndex();
}

bool Abstract::setData(const QModelIndex& index, const QVariant& value, int role)
{
	Q_UNUSED(index);
	Q_UNUSED(value);
	Q_UNUSED(role);
	return false;
}

void Abstract::beginInsertItems(int start, int end)
{
	beginInsertRows(QModelIndex(), start, end);
}

void Abstract::endInsertItems()
{
	endInsertRows();
}

void Abstract::beginRemoveItems(int start, int end)
{
	beginRemoveRows(QModelIndex(), start, end);
}

void Abstract::endRemoveItems()
{
	endRemoveRows();
}

void Abstract::changeItems(int start, int end)
{
	if (parentmodel) {
		int offset = parentmodel->offset(this);
		parentmodel->changeItems(offset + start, offset + end);
	} else {
		emit dataChanged(index(start, 0), index(end, columnCount(QModelIndex())));
	}
}

void Abstract::onStateChanged(int i)
{
	log.debug("onStateChanged: %1", i);
	changeItems(i, i);
}

void Abstract::beginSetItems(int oldcount, int newcount)
{
	if (parentmodel) {
		int parentsize = parentmodel->size();
		parentmodel->beginSetItems(parentsize, parentsize - oldcount + newcount);
	} else {
		beginRemoveItems(0, oldcount);
		endRemoveItems();
		beginInsertItems(0, newcount);
	}
}

void Abstract::endSetItems()
{
	if (parentmodel) {
		parentmodel->endSetItems();
	} else {
		endInsertItems();
	}
}
