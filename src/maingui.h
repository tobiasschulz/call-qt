#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <QResizeEvent>

#include "contactmodel.h"
#include "systemutil.h"
#include "tab.h"
#include "log.h"
#include "terminal.h"

namespace Ui
{
class Main;
}

class Main: public QMainWindow, public ID
{
Q_OBJECT

public:
	explicit Main(QWidget* parent = 0);
	~Main();
	QString id() const;

	void show();
	void resizeEvent(QResizeEvent* event);

signals:
	void shown();

public slots:
	void addTab(Tab* widget);
	void openTab(const QString& tabname);
	void openTab(Tab* widget);
	void closeTab(const QString& tabname);
	void closeTab(Tab* widget);
	void onTabChanged(int index);

	void onContactSelected(const QModelIndex & index);

	void onShowTerminalToggled(bool checked);
	void onAbout();
	void onAboutQt();

private:
	Ui::Main* ui;
	ContactModel* m_contactmodel;
	QHash<QString, Tab*> m_tabhash;
	Terminal* m_terminal;
};

#endif // MAIN_H
