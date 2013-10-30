#include "model-abstract.h"
#include "maingui.h"

using namespace Model;

Abstract::Abstract(Abstract* parentmodel, QObject* parent)
		: QAbstractItemModel(parent), m_parentmodel(parentmodel), m_showConnections(true), m_visible(true),
			m_rowOperationState(NONE)
{
	Main::instance()->settingsContactList()->listen("show-connections")->connect(this,
	SLOT(setConnectionsVisible(bool)))->pushValue();
}

int Abstract::offset(Abstract* submodel) const
{
	Q_UNUSED(submodel);
	return 0;
}

int Abstract::size() const
{
	return visible() ? internalSize() : 0;
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
	if (visible()) {
		beginInsertRows(QModelIndex(), start, end);
	}
}

void Abstract::endInsertItems()
{
	if (visible()) {
		endInsertRows();
	}
}

void Abstract::beginRemoveItems(int start, int end)
{
	if (visible()) {
		beginRemoveRows(QModelIndex(), start, end);
	}
}

void Abstract::endRemoveItems()
{
	if (visible()) {
		endRemoveRows();
	}
}

void Abstract::changeItems(int start, int end)
{
	if (visible()) {
		if (m_parentmodel) {
			int offset = m_parentmodel->offset(this);
			m_parentmodel->changeItems(offset + start, offset + end);
		} else {
			emit dataChanged(index(start, 0), index(end, columnCount(QModelIndex())));
		}
	}
}

void Abstract::onStateChanged(int i)
{
	if (visible()) {
		//log.debug("onStateChanged: %1", i);
		changeItems(i, i);
	}
}

void Abstract::beginSetItems(int oldcount, int newcount)
{
	// in child model
	if (m_parentmodel) {
		int parentsize = m_parentmodel->size();
		// log.debug("beginSetItems (child): parentsize=%1 => %1 - %2 + %3 = %4", QString::number(parentsize),
		// QString::number(oldcount), QString::number(newcount), QString::number(parentsize - oldcount + newcount));
		m_parentmodel->beginSetItems(parentsize, parentsize - oldcount + newcount);
	}
	// in parent model
	else {
		if (oldcount >= 1) {
			beginRemoveItems(0, oldcount);
			endRemoveItems();
		}
		if (newcount >= 1) {
			beginInsertItems(0, newcount);
			m_rowOperationState = INSERTING;
		}
		// log.debug("beginSetItems (parent): %1 => %2", QString::number(oldcount), QString::number(newcount));
	}
}

void Abstract::endSetItems()
{
	if (m_parentmodel) {
		m_parentmodel->endSetItems();
	} else {
		if (m_rowOperationState == INSERTING) {
			endInsertItems();
			m_rowOperationState = NONE;
		}
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

void Abstract::setVisible(bool visible)
{
	log.debug("setVisible(%1)", QString::number(visible));
	if (visible) {
		beginSetItems(size(), internalSize());
		m_visible = true;
		endSetItems();
	} else {
		beginSetItems(size(), 0);
		m_visible = false;
		endSetItems();
	}
}

