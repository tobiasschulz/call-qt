#include <QComboBox>

#include "settings-contactlist.h"
#include "ui_settings-contactlist.h"
#include "systemutil.h"
#include "config.h"

SettingsContactList::SettingsContactList()
		: Tab("Contact List", Config::instance()->icon("contact")), ui(new Ui::SettingsContactList),
			m_settings("contactlist")
{
	ui->setupUi(this);
	QObject::connect(this, SIGNAL(focus()), this, SLOT(setFocus()));

	m_settings << ButtonOption(ui->checkShowOfflineHosts, "show-offline-hosts", false);
	m_settings << ButtonOption(ui->checkShowConnections, "show-connections", true);
	m_settings << ButtonOption(ui->radioHostList, "show-hosts", false);
	m_settings << ButtonOption(ui->radioUserCompnameList, "show-users-computernames", true);
	m_settings << ButtonOption(ui->radioUserList, "show-users", false);
	m_settings << ButtonOption(ui->radioUserNameSystem, "user-name-format-system", false);
	m_settings << ButtonOption(ui->radioUserNameFirst, "user-name-format-first", true);
	m_settings << ButtonOption(ui->radioUserNameFull, "user-name-format-full", false);
	m_settings.loadSettings();
}

SettingsContactList::~SettingsContactList()
{
	delete ui;
}

QString SettingsContactList::id() const
{
	return "SettingsContactList";
}

Settings* SettingsContactList::settings()
{
	return &m_settings;
}

