/*
 * settings.h
 *
 *  Created on: 26.10.2013
 *      Author: tobias
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QList>
#include <QAbstractButton>
#include <QPointer>
#include <QSignalMapper>

#include "id.h"

class Option;
class Settings;
class OptionCatcher;

class Option: public QObject
{
Q_OBJECT
public:
	explicit Option(QAbstractButton* button, QString name, QVariant defaultValue);
	Option(const Option& other);
	Option& operator=(const Option& other);
	bool operator==(const Option& other) const;
	bool operator!=(const Option& other) const;

	QAbstractButton* button() const;
	QString name() const;
	QVariant defaultValue() const;

private:
	QPointer<QAbstractButton> m_button;
	QString m_name;
	QVariant m_defaultValue;
};

class Settings: public QObject, public ID
{
Q_OBJECT
public:
	explicit Settings(QString group = QString());
	QString id() const;

	void addOption(Option option);
	void loadSettings();
	void saveSettings();
	OptionCatcher* listen(QString name);

signals:
	void booleanOptionChanged(QString name, bool value);

public slots:

private slots:
	void onOptionChanged(int);

private:
	QList<Option> m_options;
	QPointer<QSignalMapper> m_mapper;
	QString m_group;
};

class OptionCatcher: public QObject, public ID
{
Q_OBJECT
public:
	explicit OptionCatcher(Settings* settings, QString name);
	QString id() const;

signals:
	void booleanOptionChanged(bool value);

private slots:
	void onBooleanOptionChanged(QString name, bool value);

private:
	QPointer<Settings> m_settings;
	QString m_name;
};

Settings& operator<<(Settings& settings, const Option& option);

#endif /* SETTINGS_H */
