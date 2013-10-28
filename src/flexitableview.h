#ifndef FLEXITABLEVIEW_H
#define FLEXITABLEVIEW_H

#include <QTableView>

class FlexiTableView: public QTableView
{
Q_OBJECT

public:
	FlexiTableView(QWidget * parent = 0);
	~FlexiTableView();

	virtual QSize sizeHint() const;
};

#endif // FLEXITABLEVIEW_H
