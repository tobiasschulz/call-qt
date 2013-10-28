#include "flexitableview.h"

FlexiTableView::FlexiTableView(QWidget * parent)
		: QTableView(parent)
{
}
FlexiTableView::~FlexiTableView()
{
}

QSize FlexiTableView::sizeHint() const
{
	QSize size = QTableView::sizeHint();
	size.setWidth(150);
	return size;
}
