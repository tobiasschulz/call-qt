#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QAudioFormat>
#include <QPointer>

#include "contactmodel.h"
#include "systemutil.h"
#include "tab.h"
#include "id.h"
#include "tabs.h"
#include "terminal.h"
#include "audiodevices.h"
#include "settings-contactlist.h"

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

	static Tabs* tabs();

	void show();
	void init();
	void resizeEvent(QResizeEvent* event);

	Settings* settingsContactList();

signals:
	void shown();
	void volumeChangedInput(qreal volume);
	void volumeChangedOutput(qreal volume);

public slots:
	void onContactSelected(const QModelIndex & index);
	void onTabTitleChanged(QString);

	void showStats();
	void hideStats();
	void onShowStatsToggled(bool checked);
	void onStatsContact(Contact contact);
	void onStatsDurationInput(int ms);
	void onStatsDurationOutput(int ms);
	void onStatsLatencyInput(int ms);
	void onStatsLatencyOutput(int ms);
	void onStatsLevelInput(qreal level);
	void onStatsLevelOutput(qreal level);
	void onStatsFormatInput(QAudioFormat format);
	void onStatsFormatOutput(QAudioFormat format);
	void onSliderVolumeInput(int value);
	void onSliderVolumeOutput(int value);

	void onMenuShowTerminal();
	void onMenuAudioDevices();
	void onMenuSettingsContactList();
	void onMenuAbout();
	void onMenuAboutQt();
	void onMenuAddContact();

private:
	explicit Main(QWidget* parent = 0);
	Main(const Main &); // hide copy constructor
	Main& operator=(const Main &); // hide assign op
	// we leave just the declarations, so the compiler will warn us
	// if we try to use those two functions by accident
	static QPointer<Main> m_instance;

	Ui::Main* ui;
	QPointer<Tabs> m_tabs;
	QPointer<ContactModel> m_contactmodel;
	QPointer<Terminal> m_terminal;
	QPointer<AudioDevices> m_audiodevices;
	QPointer<SettingsContactList> m_settings_contactlist;
	bool m_statsVisible;
};

#endif // MAIN_H
