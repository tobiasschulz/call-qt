#ifndef SETTINGS_CONTACTLIST_H
#define SETTINGS_CONTACTLIST_H

#include <QWidget>
#include <QComboBox>
#include <QString>
#include <QMutexLocker>
#include <QMutex>

#include "tab.h"
#include "settings.h"

namespace Ui
{
	class SettingsContactList;
}

class SettingsContactList: public Tab
{
Q_OBJECT

public:
	explicit SettingsContactList();
	~SettingsContactList();
	QString id() const;

	Settings* settings();

signals:
	void showOfflineHostsToggled(bool state);
	void showConnectionsToggled(bool state);
	void showHostsToggled(bool state);
	void showUsersToggled(bool state);

public slots:

private:
	Ui::SettingsContactList *ui;
	Settings m_settings;
};

#endif // SETTINGS_CONTACTLIST_H
