/*
 * settings.cpp
 *
 *  Created on: 26.10.2013
 *      Author: tobias
 */

#include <QSettings>
#include <QMutexLocker>

#include "settings.h"

ButtonListener::ButtonListener(QAbstractButton* button, QString name, QObject* parent)
		: QObject(parent), m_name(name)
{
	QObject::connect(button, SIGNAL(toggled(bool)), this, SLOT(onOptionChanged(bool)));
}
QString ButtonListener::id() const
{
	return "ButtonOption<" + m_name + ">";
}
void ButtonListener::onOptionChanged(bool value)
{
	emit optionChanged(m_name, value);
}

ButtonOption::ButtonOption(QAbstractButton* button, QString name, QVariant defaultValue)
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
QVariant ButtonOption::defaultValue() const
{
	return m_defaultValue;
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

void Settings::onBooleanOptionChanged(QString name, bool value)
{
	emit booleanOptionChanged(name, value);
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
		option.button()->setChecked(settings.value(option.name(), option.defaultValue()).toBool());
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
			settings.setValue(option.name(), option.button()->isChecked());
			log.debug("saveSettings: %1 = %2", option.name(), QVariant(option.button()->isChecked()).toString());
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
			bool value = option.button()->isChecked();
			log.debug("pushValue: %1 = %2", name, value);
			emit booleanOptionChanged(name, value);
		}
	}
}

OptionCatcher::OptionCatcher(Settings* settings, QString name)
		: QObject(settings), m_settings(settings), m_name(name)
{
	QObject::connect(settings, &Settings::booleanOptionChanged, this, &OptionCatcher::onBooleanOptionChanged);
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

OptionCatcher* OptionCatcher::connect(const QObject* obj, const char* method)
{
	QObject::connect(this, SIGNAL(booleanOptionChanged(bool)), obj, method);
	return this;
}

OptionCatcher* OptionCatcher::pushValue()
{
	m_settings->pushValue(m_name);
	return this;
}

