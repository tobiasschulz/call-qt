#ifndef TABS_H
#define TABS_H

#include <QTabWidget>
#include <QString>
#include <QPointer>
#include <QTimer>
#include <QIcon>

#include "id.h"
#include "tab.h"
#include "contact.h"

class Tabs: public QObject, public ID
{
Q_OBJECT

public:
	Tabs(QTabWidget* m_tabs);
	QString id() const;

signals:
	void tabTitleChanged(QString);
	void contactTabAvailable(Contact contact);

public:
	int addTab(Tab* widget);
	void openTab(Tab* widget);
	void closeTab(Tab* widget);

public slots:
	void openTab(const QString& tabname);
	void closeTab(const QString& tabname);
	void closeTab(int index);
	void onTabChanged(int index);
	void onTabIconChanged();

	void addContactTab(Contact contact);
	void openContactTab(Contact contact);

private:
	QPointer<QTabWidget> m_tabs;
	QHash<QString, Tab*> m_tabhash;
};

#endif // TABS_H
