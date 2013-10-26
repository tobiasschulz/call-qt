/*
 * settings.cpp
 *
 *  Created on: 26.10.2013
 *      Author: tobias
 */

#include <QSettings>

#include "settings.h"

Option::Option(QAbstractButton* button, QString name, QVariant defaultValue)
		: QObject(), m_button(button), m_name(name), m_defaultValue(defaultValue)
{
}
Option::Option(const Option& other)
		: QObject(), m_button(other.m_button), m_name(other.m_name), m_defaultValue(other.m_defaultValue)
{
}
Option& Option::operator=(const Option& other)
{
	m_button = other.m_button;
	m_name = other.m_name;
	m_defaultValue = other.m_defaultValue;
	return *this;
}
bool Option::operator==(const Option& other) const
{
	return m_button == other.m_button, m_name == other.m_name && m_defaultValue == other.m_defaultValue;
}
bool Option::operator!=(const Option& other) const
{
	return !(*this == other);
}

QAbstractButton* Option::button() const
{
	return m_button;
}
QString Option::name() const
{
	return m_name;
}
QVariant Option::defaultValue() const
{
	return m_defaultValue;
}

Settings::Settings(QString group)
		: QObject(), m_options(), m_mapper(new QSignalMapper(this)), m_group(group)
{
	QObject::connect(m_mapper, SIGNAL(mapped(int)), this, SLOT(onOptionChanged(int)));
}

QString Settings::id() const
{
	return m_group.size() > 0 ? "Settings<" + m_group + ">" : "Settings";
}

void Settings::addOption(Option option)
{
	m_options << option;
	QObject::connect(option.button(), SIGNAL(clicked()), m_mapper, SLOT(map()));
	m_mapper->setMapping((QObject*) option.button(), m_options.indexOf(option));
}

Settings& operator<<(Settings& settings, const Option& option)
{
	settings.addOption(option);
	return settings;
}

void Settings::onOptionChanged(int index)
{
	Option& option = m_options[index];
	log.debug("booleanOptionChanged: name=%1, value=%2", option.name(), QString::number(option.button()->isChecked()));
	emit booleanOptionChanged(option.name(), option.button()->isChecked());
	saveSettings();
}

void Settings::loadSettings()
{
	QSettings settings;
	if (m_group.size() > 0)
		settings.beginGroup(m_group);
	foreach (const Option& option, m_options)
	{
		option.button()->setChecked(settings.value(option.name(), option.defaultValue()).toBool());
	}
	if (m_group.size() > 0)
		settings.endGroup();
}

void Settings::saveSettings()
{
	QSettings settings;
	if (m_group.size() > 0)
		settings.beginGroup(m_group);
	foreach (const Option& option, m_options)
	{
		settings.setValue(option.name(), option.button()->isChecked());
	}
	if (m_group.size() > 0)
		settings.endGroup();
}

OptionCatcher* Settings::listen(QString name)
{
	return new OptionCatcher(this, name);
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

