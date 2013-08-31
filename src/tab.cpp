#include "tab.h"

Tab::Tab(QString tabname, QIcon tabicon)
		: QWidget(), m_tabname(tabname), m_tabicon(tabicon)
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

void Tab::opened()
{
	log.debug("opened()");
}

void Tab::closed()
{
	log.debug("closed()");
}

