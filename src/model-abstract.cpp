#include "model-abstract.h"
#include "maingui.h"

using namespace Model;

Abstract::Abstract(Abstract* parentmodel, QObject* parent)
		: QAbstractItemModel(parent), m_parentmodel(parentmodel), m_showConnections(true), m_visible(true)
{
	QObject::connect(Main::instance()->settingsContactList()->listen("show-connections"),
			&OptionCatcher::booleanOptionChanged, this, &Abstract::setConnectionsVisible);
}

int Abstract::offset(Abstract* submodel) const
{
	Q_UNUSED(submodel);
	return 0;
}

int Abstract::rowCount(const QModelIndex& parent) const
{
	return !parent.isValid() && visible() ? size() : 0;
}

int Abstract::columnCount(const QModelIndex& parent) const
{
	return !parent.isValid() ? 2 : 0;
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
	if (m_parentmodel) {
		int offset = m_parentmodel->offset(this);
		m_parentmodel->changeItems(offset + start, offset + end);
	} else {
		emit dataChanged(index(start, 0), index(end, columnCount(QModelIndex())));
	}
}

void Abstract::onStateChanged(int i)
{
	//log.debug("onStateChanged: %1", i);
	changeItems(i, i);
}

void Abstract::beginSetItems(int oldcount, int newcount)
{
	if (m_parentmodel) {
		int parentsize = m_parentmodel->size();
		m_parentmodel->beginSetItems(parentsize, parentsize - oldcount + newcount);
	} else {
		beginRemoveItems(0, oldcount);
		endRemoveItems();
		beginInsertItems(0, newcount);
	}
}

void Abstract::endSetItems()
{
	if (m_parentmodel) {
		m_parentmodel->endSetItems();
	} else {
		endInsertItems();
	}
}

void Abstract::setConnectionsVisible(bool show)
{
	m_showConnections = show;
}

bool Abstract::visible() const
{
	return m_visible;
}

void Abstract::setVisible(bool state)
{
	beginSetItems(size(), 0);
	endSetItems();
	m_visible = state;
	beginSetItems(0, size());
	endSetItems();
}

