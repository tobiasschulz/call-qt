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
#include <QMutex>

#include "id.h"

class ButtonOption;
class Settings;
class OptionCatcher;
class ToggleStateListener;

class ButtonListener: public QObject, public ID
{
Q_OBJECT
public:
	explicit ButtonListener(QAbstractButton* button, QString optionname, QObject* parent = 0);
	explicit ButtonListener(QAbstractButton* button, QString optionname, QString valuename, QObject* parent = 0);
	QString id() const;

signals:
	void optionChanged(QString name, bool value);
	void optionChanged(QString name, QString valuename);

private slots:
	void onOptionChanged(bool value);

private:
	QString m_optionname;
	QString m_valuename;
};

class ButtonOption: public QObject, public ID
{
Q_OBJECT
public:
	explicit ButtonOption(QAbstractButton* button, QString name, bool defaultValue);
	ButtonOption(const ButtonOption& other);
	ButtonOption& operator=(const ButtonOption& other);
	QString id() const;
	bool operator==(const ButtonOption& other) const;
	bool operator!=(const ButtonOption& other) const;

	QAbstractButton* button() const;
	QString name() const;
	bool defaultValue() const;
	bool value() const;
	void setValue(bool value) const;

private:
	QPointer<QAbstractButton> m_button;
	QString m_name;
	bool m_defaultValue;
};

class ButtonGroup: public QObject, public ID
{
Q_OBJECT
public:
	explicit ButtonGroup(Settings* settings, QString optionname);
	ButtonGroup(const ButtonGroup& other);
	ButtonGroup& operator=(const ButtonGroup& other);
	QString id() const;
	bool operator==(const ButtonGroup& other) const;
	bool operator!=(const ButtonGroup& other) const;

	QString optionname() const;
	QString defaultValue() const;
	QString value() const;
	void setValue(QString value) const;
	void addOption(ButtonOption option);

private:
	static QHash<ButtonGroup, QList<ButtonOption>> m_buttonoptions;
	QString m_optionname;
	QPointer<Settings> m_settings;
};

class Settings: public QObject, public ID
{
Q_OBJECT
public:
	explicit Settings(QString group = QString());
	QString id() const;

	QList<ButtonOption> options() const;
	void addOption(ButtonOption option);
	void addGroup(ButtonGroup group);
	void loadSettings();
	void saveSettings();
	OptionCatcher* listen(QString name);
	void pushValue(QString name);

signals:
	void booleanOptionChanged(QString name, bool value);
	void stringOptionChanged(QString name, QString value);

public slots:

private slots:
	void onBooleanOptionChanged(QString name, bool value);
	void onStringOptionChanged(QString name, QString value);

private:
	QList<ButtonOption> m_buttonoptions;
	QList<ButtonGroup> m_buttongroups;
	QString m_group;
	QMutex m_settingslock;
};

class OptionCatcher: public QObject, public ID
{
Q_OBJECT
public:
	explicit OptionCatcher(Settings* settings, QString name);
	QString id() const;

	void setOption(ButtonOption* option);
	OptionCatcher* connect(const QObject* obj, const char* method);
	OptionCatcher* pushValue();

signals:
	void booleanOptionChanged(bool value);
	void stringOptionChanged(QString value);

private slots:
	void onBooleanOptionChanged(QString name, bool value);
	void onStringOptionChanged(QString name, QString value);

private:
	QPointer<Settings> m_settings;
	QString m_name;
};

Settings& operator<<(Settings& settings, const ButtonOption& option);
ButtonGroup& operator<<(ButtonGroup& group, const ButtonOption& option);

#endif /* SETTINGS_H */
