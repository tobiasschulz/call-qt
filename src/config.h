/*
 * config.h
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QIcon>
#include <QAudioFormat>
#include <QAudioDeviceInfo>

#include "contact.h"
#include "id.h"

class Config: public QObject, public ID
{
Q_OBJECT

public:
	static Config* instance();
	QString id() const;

	QStringList localhosts();
	QStringList defaultHostnames();
	QList<Host> defaultHosts();
	long uid();
	long uptime();
	QIcon icon(QString iconname);
	QString version();
	QString build();
	void addLocalhost(Host host);
	bool isLocalhost(QString host);
	QAudioFormat currentAudioFormat();
	QAudioFormat chooseAudioFormat(int freq, int channels, int samplesize);
	QAudioDeviceInfo currentAudioInputDevice();
	QAudioDeviceInfo currentAudioOutputDevice();

	int DEFAULT_PORT;
	int SOCKET_READ_TIMEOUT;
	int SOCKET_CONNECT_TIMEOUT;
	int CONTACT_SCAN_INTERVAL;

	QString DEFAULT_CONTACT_HOSTS[];

private:
	void readConfigLocalhosts();
	void writeConfigLocalhosts();

	QStringList m_localhosts;
	long m_uid;
	long m_uptime;
	QAudioFormat m_audioinputformat;
    QAudioDeviceInfo m_audioinputdevice;
    QAudioDeviceInfo m_audiooutputdevice;

	explicit Config(QObject *parent = 0);
	Config(const Config &); // hide copy constructor
	Config& operator=(const Config &); // hide assign op
	// we leave just the declarations, so the compiler will warn us
	// if we try to use those two functions by accident
	static Config* m_instance;

};

#endif /* CONFIG_H */
