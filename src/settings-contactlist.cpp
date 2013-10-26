#include <QComboBox>

#include "settings-contactlist.h"
#include "ui_settings-contactlist.h"
#include "systemutil.h"
#include "config.h"

SettingsContactList::SettingsContactList()
		: Tab("Contact List", Config::instance()->icon("microphone")), ui(new Ui::SettingsContactList),
			m_settings("contactlist")
{
	ui->setupUi(this);
	QObject::connect(this, SIGNAL(focus()), this, SLOT(setFocus()));

	m_settings << Option(ui->checkShowOfflineHosts, "show-offline-hosts", false);
	m_settings << Option(ui->checkShowConnections, "show-connections", true);
	m_settings << Option(ui->radioHostList, "show-hosts", false);
	m_settings << Option(ui->radioUserList, "show-users", true);
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

