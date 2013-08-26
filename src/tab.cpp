#include "tab.h"

Tab::Tab(QString tabname, QIcon tabicon, QWidget *parent)
		: QWidget(parent), m_tabname(tabname), m_tabicon(tabicon)
{
}

QString Tab::tabname() const
{
	return m_tabname;
}
QIcon Tab::tabicon() const
{
	return m_tabicon;
}
