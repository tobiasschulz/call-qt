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
	explicit ButtonListener(QAbstractButton* button, QString name, QObject* parent = 0);
	QString id() const;

signals:
	void optionChanged(QString name, bool value);

private slots:
	void onOptionChanged(bool value);

private:
	QString m_name;
};

class ButtonOption: public QObject, public ID
{
Q_OBJECT
public:
	explicit ButtonOption(QAbstractButton* button, QString name, QVariant defaultValue);
	ButtonOption(const ButtonOption& other);
	ButtonOption& operator=(const ButtonOption& other);
	QString id() const;
	bool operator==(const ButtonOption& other) const;
	bool operator!=(const ButtonOption& other) const;

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

	QList<ButtonOption> options() const;
	void addOption(ButtonOption option);
	void loadSettings();
	void saveSettings();
	OptionCatcher* listen(QString name);
	void pushValue(QString name);

signals:
	void booleanOptionChanged(QString name, bool value);

public slots:

private slots:
	void onBooleanOptionChanged(QString name, bool value);

private:
	QList<ButtonOption> m_buttonoptions;
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

private slots:
	void onBooleanOptionChanged(QString name, bool value);

private:
	QPointer<Settings> m_settings;
	QString m_name;
};

Settings& operator<<(Settings& settings, const ButtonOption& option);

#endif /* SETTINGS_H */
