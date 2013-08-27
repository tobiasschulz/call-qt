#ifndef TAB_H
#define TAB_H

#include <QWidget>
#include <QString>
#include <QIcon>

#include "id.h"

class Tab: public QWidget, public ID
{
Q_OBJECT

public:
	Tab(QString tabname = QString(), QIcon tabicon = QIcon(), QWidget *parent = 0);

	virtual QString tabname() const;
	virtual QIcon tabicon() const;

	virtual QString id() const = 0;

signals:
	void focus();

private:
	const QString m_tabname;
	const QIcon m_tabicon;
};

#endif // TAB_H
