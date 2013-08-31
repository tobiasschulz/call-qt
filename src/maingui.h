#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <QResizeEvent>

#include "contactmodel.h"
#include "systemutil.h"
#include "tab.h"
#include "id.h"
#include "terminal.h"
#include "audiodevices.h"

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

public:
	int addTab(Tab* widget);
	void openTab(Tab* widget);
	void closeTab(Tab* widget);

public slots:
	void openTab(const QString& tabname);
	void closeTab(const QString& tabname);
	void closeTab(int index);
	void onTabChanged(int index);

	void onContactSelected(const QModelIndex & index);
	void addContactTab(Contact contact);
	void openContactTab(Contact contact);
	void onTabIconChanged();

	void showStats();
	void hideStats();
	void onShowStatsToggled(bool checked);
	void onStatsDurationInput(int ms);
	void onStatsDurationOutput(int ms);
	void onStatsLatencyInput(int ms);
	void onStatsLatencyOutput(int ms);
	void onStatsLevelInput(qreal level);
	void onStatsLevelOutput(qreal level);

	void onMenuShowTerminal();
	void onMenuAudioDevices();
	void onMenuAbout();
	void onMenuAboutQt();

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
	AudioDevices* m_audiodevices;
	bool statsVisible;
};

#endif // MAIN_H
