/*
 * settings.cpp
 *
 *  Created on: 26.10.2013
 *      Author: tobias
 */

#include <QSettings>
#include <QMutexLocker>

#include "settings.h"

QHash<ButtonGroup, QList<ButtonOption>> ButtonGroup::m_buttonoptions;

ButtonListener::ButtonListener(QAbstractButton* button, QString optionname, QObject* parent)
		: QObject(parent), m_optionname(optionname), m_valuename()
{
	QObject::connect(button, SIGNAL(toggled(bool)), this, SLOT(onOptionChanged(bool)));
}

ButtonListener::ButtonListener(QAbstractButton* button, QString optionname, QString valuename, QObject* parent)
		: QObject(parent), m_optionname(optionname), m_valuename(valuename)
{
	QObject::connect(button, SIGNAL(toggled(bool)), this, SLOT(onOptionChanged(bool)));
}
QString ButtonListener::id() const
{
	return "ButtonOption<" + m_optionname + ">";
}
void ButtonListener::onOptionChanged(bool value)
{
	emit optionChanged(m_optionname, value);
	if (value && !m_valuename.isEmpty()) {
		emit optionChanged(m_optionname, m_valuename);
	}
}

ButtonOption::ButtonOption(QAbstractButton* button, QString name, bool defaultValue)
		: QObject(), m_button(button), m_name(name), m_defaultValue(defaultValue)
{
}
ButtonOption::ButtonOption(const ButtonOption& other)
		: QObject(), m_button(other.m_button), m_name(other.m_name), m_defaultValue(other.m_defaultValue)
{
}
ButtonOption& ButtonOption::operator=(const ButtonOption& other)
{
	m_button = other.m_button;
	m_name = other.m_name;
	m_defaultValue = other.m_defaultValue;
	return *this;
}
QString ButtonOption::id() const
{
	return "ButtonOption<" + m_name + ">";
}

bool ButtonOption::operator==(const ButtonOption& other) const
{
	return m_button == other.m_button, m_name == other.m_name && m_defaultValue == other.m_defaultValue;
}
bool ButtonOption::operator!=(const ButtonOption& other) const
{
	return !(*this == other);
}

QAbstractButton* ButtonOption::button() const
{
	return m_button;
}
QString ButtonOption::name() const
{
	return m_name;
}
bool ButtonOption::defaultValue() const
{
	return m_defaultValue;
}
bool ButtonOption::value() const
{
	return m_button->isChecked();
}
void ButtonOption::setValue(bool value) const
{
	m_button->setChecked(value);
}

ButtonGroup::ButtonGroup(Settings* settings, QString optionname)
		: QObject(), m_optionname(optionname), m_settings(settings)
{
	if (settings) {
		settings->addGroup(*this);
	}
}
ButtonGroup::ButtonGroup(const ButtonGroup& other)
		: QObject(), m_optionname(other.m_optionname), m_settings(other.m_settings)
{
}
ButtonGroup& ButtonGroup::operator=(const ButtonGroup& other)
{
	m_optionname = other.m_optionname;
	m_settings = other.m_settings;
	return *this;
}
QString ButtonGroup::id() const
{
	return "ButtonGroup<" + m_optionname + ">";
}
bool ButtonGroup::operator==(const ButtonGroup& other) const
{
	return m_optionname == other.m_optionname;
}
bool ButtonGroup::operator!=(const ButtonGroup& other) const
{
	return !(*this == other);
}

QString ButtonGroup::optionname() const
{
	return m_optionname;
}
QString ButtonGroup::defaultValue() const
{
	foreach (const ButtonOption& option, m_buttonoptions[*this])
	{
		if (option.defaultValue()) {
			return option.name();
		}
	}
	return QString();
}
QString ButtonGroup::value() const
{
	foreach (const ButtonOption& option, m_buttonoptions[*this])
	{
		if (option.value() == true) {
			return option.name();
		}
	}
	return QString();
}
void ButtonGroup::setValue(QString value) const
{
	foreach (const ButtonOption& option, m_buttonoptions[*this])
	{
		if (option.name() == value) {
			return option.setValue(true);
		} else {
			return option.setValue(false);
		}
	}
}

void ButtonGroup::addOption(ButtonOption option)
{
	if (m_settings) {
		m_buttonoptions[*this] << option;
		ButtonListener* listener = new ButtonListener(option.button(), m_optionname, option.name(), m_settings.data());
		QObject::connect(listener, SIGNAL(optionChanged(QString, QString)), m_settings.data(),
		SLOT(onStringOptionChanged(QString, QString)));
	} else {
		log.error("addOption(%1): m_settings in NULL!", Log::print(option));
	}
}

ButtonGroup& operator<<(ButtonGroup& group, const ButtonOption& option)
{
	group.addOption(option);
	return group;
}

Settings::Settings(QString group)
		: QObject(), m_buttonoptions(), m_group(group), m_settingslock()
{
}

QString Settings::id() const
{
	return m_group.size() > 0 ? "Settings<" + m_group + ">" : "Settings";
}

QList<ButtonOption> Settings::options() const
{
	return m_buttonoptions;
}

void Settings::addOption(ButtonOption option)
{
	m_buttonoptions << option;
	ButtonListener* listener = new ButtonListener(option.button(), option.name(), this);
	QObject::connect(listener, SIGNAL(optionChanged(QString, bool)), this, SLOT(onBooleanOptionChanged(QString, bool)));
}

Settings& operator<<(Settings& settings, const ButtonOption& option)
{
	settings.addOption(option);
	return settings;
}

void Settings::addGroup(ButtonGroup group)
{
	m_buttongroups << group;
}

void Settings::onBooleanOptionChanged(QString name, bool value)
{
	emit booleanOptionChanged(name, value);
	saveSettings();
}

void Settings::onStringOptionChanged(QString name, QString value)
{
	emit stringOptionChanged(name, value);
	saveSettings();
}

void Settings::loadSettings()
{
	QMutexLocker locker(&m_settingslock);
	QSettings settings;
	if (m_group.size() > 0)
		settings.beginGroup(m_group);
	foreach (const ButtonOption& option, m_buttonoptions)
	{
		log.debug("loadSettings: %1 = %2", option.name(), settings.value(option.name()).toString());
		option.setValue(settings.value(option.name(), option.defaultValue()).toBool());
	}
	foreach (const ButtonGroup& group, m_buttongroups)
	{
		log.debug("loadSettings: %1 = %2", group.optionname(), settings.value(group.optionname()).toString());
		group.setValue(settings.value(group.optionname(), group.defaultValue()).toString());
	}
	if (m_group.size() > 0)
		settings.endGroup();
}

void Settings::saveSettings()
{
	if (m_settingslock.tryLock()) {
		QSettings settings;
		if (m_group.size() > 0)
			settings.beginGroup(m_group);
		foreach (const ButtonOption& option, m_buttonoptions)
		{
			log.debug("saveSettings: %1 = %2", option.name(), QVariant(option.value()).toString());
			settings.setValue(option.name(), option.value());
		}
		foreach (const ButtonGroup& group, m_buttongroups)
		{
			log.debug("saveSettings: %1 = %2", group.optionname(), group.value());
			settings.setValue(group.optionname(), group.value());
		}
		if (m_group.size() > 0)
			settings.endGroup();
		m_settingslock.unlock();
	}
}

OptionCatcher* Settings::listen(QString name)
{
	Settings* tmp = this;
	return new OptionCatcher(tmp, name);
}

void Settings::pushValue(QString name)
{
	foreach (const ButtonOption& option, m_buttonoptions)
	{
		if (option.name() == name) {
			bool value = option.value();
			log.debug("pushValue: %1 = %2", name, value);
			emit booleanOptionChanged(name, value);
		}
	}
	foreach (const ButtonGroup& group, m_buttongroups)
	{
		if (group.optionname() == name) {
			QString value = group.value();
			log.debug("pushValue: %1 = %2", name, value);
			emit stringOptionChanged(name, value);
		}
	}
}

OptionCatcher::OptionCatcher(Settings* settings, QString name)
		: QObject(settings), m_settings(settings), m_name(name)
{
	QObject::connect(settings, &Settings::booleanOptionChanged, this, &OptionCatcher::onBooleanOptionChanged);
	QObject::connect(settings, &Settings::stringOptionChanged, this, &OptionCatcher::onStringOptionChanged);
}
QString OptionCatcher::id() const
{
	return "OptionCatcher<" + m_name + ">";
}

void OptionCatcher::onBooleanOptionChanged(QString name, bool value)
{
	if (name == m_name) {
		emit booleanOptionChanged(value);
	}
}

void OptionCatcher::onStringOptionChanged(QString name, QString value)
{
	if (name == m_name) {
		emit stringOptionChanged(value);
	}
}

OptionCatcher* OptionCatcher::connect(const QObject* obj, const char* method)
{
	QString methodStr(QString::fromLocal8Bit(method));
	log.debug("connect(%1)", methodStr);
	if (methodStr.contains("bool"))
		QObject::connect(this, SIGNAL(booleanOptionChanged(bool)), obj, method);
	else
		QObject::connect(this, SIGNAL(stringOptionChanged(QString)), obj, method);
	return this;
}

OptionCatcher* OptionCatcher::pushValue()
{
	m_settings->pushValue(m_name);
	return this;
}

