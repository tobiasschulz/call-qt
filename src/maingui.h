#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <QResizeEvent>

#include "contactmodel.h"
#include "systemutil.h"
#include "tab.h"
#include "id.h"
#include "terminal.h"

namespace Ui
{
class Main;
}

class Main: public QMainWindow, public ID
{
Q_OBJECT

public:
	static Main* instance();
	~Main();
	QString id() const;

	void show();
	void resizeEvent(QResizeEvent* event);

signals:
	void shown();
	void contactTabAvailable(Contact contact);

public slots:
	void addTab(Tab* widget);
	void openTab(const QString& tabname);
	void openTab(Tab* widget);
	void closeTab(const QString& tabname);
	void closeTab(Tab* widget);
	void onTabChanged(int index);

	void onContactSelected(const QModelIndex & index);
	void addContactTab(Contact contact);
	void openContactTab(Contact contact);
	void onTabIconChanged();

	void onShowTerminalToggled(bool checked);
	void onAbout();
	void onAboutQt();

private:
	explicit Main(QWidget* parent = 0);
	Main(const Main &); // hide copy constructor
	Main& operator=(const Main &); // hide assign op
	// we leave just the declarations, so the compiler will warn us
	// if we try to use those two functions by accident
	static Main* m_instance;

	Ui::Main* ui;
	ContactModel* m_contactmodel;
	QHash<QString, Tab*> m_tabhash;
	Terminal* m_terminal;
};

#endif // MAIN_H
