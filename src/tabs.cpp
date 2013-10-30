#include "tabs.h"
#include "chattab.h"

Tabs::Tabs(QTabWidget* tabs)
		: QObject(), m_tabs(tabs), m_tabset()
{
	QObject::connect(m_tabs.data(), &QTabWidget::currentChanged, this, &Tabs::onTabChanged);
	QObject::connect(m_tabs.data(), SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

QString Tabs::id() const
{
	return "Tabs";
}

int Tabs::addTab(Tab* widget)
{
	int index = -1;
	if (widget) {
		index = m_tabs->indexOf(widget);
		if (index == -1) {
			m_tabset << widget;
			m_tabs->setUpdatesEnabled(false);
			index = m_tabs->addTab(widget, widget->tabicon(), widget->tabname());
			m_tabs->setUpdatesEnabled(true);
			QObject::connect(widget, &Tab::tabIconChanged, this, &Tabs::onTabIconChanged, Qt::UniqueConnection);
			QObject::connect(widget, &Tab::tabNameChanged, this, &Tabs::onTabNameChanged, Qt::UniqueConnection);
		}
	}
	return index;
}

void Tabs::_openTab(const QString& tabname)
{
	/*	if (m_tabhash.contains(tabname)) {
	 openTab (m_tabhash[tabname]);
	 }*/
}

void Tabs::openTab(Tab* widget)
{
	if (!m_tabset.contains(widget)) {
		addTab(widget);
	}

	m_tabs->setUpdatesEnabled(false);
	int index = m_tabs->indexOf(widget);
	if (index == -1) {
		index = addTab(widget);
		m_tabs->setCurrentIndex(index);
	} else if (index == m_tabs->currentIndex()) {
		// already selected
		emit widget->focus();
	} else {
		m_tabs->setCurrentIndex(index);
	}
	widget->opened();
	m_tabs->setUpdatesEnabled(true);
}

void Tabs::_closeTab(const QString& tabname)
{
	/*if (m_tabhash.contains(tabname)) {
	 log.debug("close tab <tabname=%1>...", tabname);
	 closeTab (m_tabhash[tabname]);
	 } else {
	 log.debug("close tab <tabname=%1>: error: tabname not found", tabname);
	 }*/
}

void Tabs::closeTab(Tab* widget)
{
	m_tabs->setUpdatesEnabled(false);
	int index = m_tabs->indexOf(widget);
	if (index != -1) {
		log.debug("close tab <widget=%1>...", widget->metaObject()->className());
		m_tabs->removeTab(index);
		widget->closed();
	} else {
		log.debug("close tab <widget=%1>: error: widget not found.", widget->metaObject()->className());
	}
	m_tabs->setUpdatesEnabled(true);
}

void Tabs::closeTab(int index)
{
	Tab* widget = (Tab*) m_tabs->widget(index);
	if (widget != 0) {
		closeTab(widget);
	}
}

void Tabs::onTabChanged(int index)
{
	if (index != -1) {
		QWidget* widget = m_tabs->widget(index);
		if (widget) {
			Tab* tab = (Tab*) widget;
			emit tab->focus();
			emit tabTitleChanged(tab->tabname());
		}
	}
}

void Tabs::addContactTab(Contact contact)
{
	ChatTab* chattab = ChatTab::instance(contact);
	if (contact != Contact::INVALID_CONTACT) {
		log.debug("add contact tab: %1 (tab: %2)", contact.id(), chattab->id());
		addTab(chattab);
		emit contactTabAvailable(contact);
	}
}

void Tabs::openContactTab(Contact contact)
{
	if (contact != Contact::INVALID_CONTACT) {
		ChatTab* chattab = ChatTab::instance(contact);
		log.debug("open contact tab: %1 (tab: %2)", contact.id(), chattab->id());
		chattab->setContact(contact);
		openTab(chattab);
	}
}

void Tabs::onTabIconChanged()
{
	for (int i = 0; i < m_tabs->count(); ++i) {
		Tab* widget = qobject_cast<Tab*>(m_tabs->widget(i));
		if (widget) {
			m_tabs->setTabIcon(i, widget->tabicon());
		}
	}
}

void Tabs::onTabNameChanged()
{
	for (int i = 0; i < m_tabs->count(); ++i) {
		Tab* widget = qobject_cast<Tab*>(m_tabs->widget(i));
		if (widget) {
			m_tabs->setTabText(i, widget->tabname());
		}
	}
}

