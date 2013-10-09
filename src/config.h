/*
 * config.h
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QList>
#include <QStringList>
#include <QIcon>
#include <QMovie>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QMutex>
#include <QMutexLocker>

#include "contact.h"
#include "id.h"

class Config: public QObject, public ID
{
Q_OBJECT

public:
	static Config* instance();
	QString id() const;

	// general
	long uid();
	long uptime();
	QIcon icon(QString name, QString ext = "png");
	QMovie* movie(QString name, QString ext = "gif");
	QString version();
	QString build();

	enum HostType
	{
		LOCALHOST, KNOWN_HOST, UNKNOWN_HOST
	};

	// host-related
	QStringList defaultHostnames();
	QList<quint16> defaultPorts();
	QList<Host> defaultHosts();
	QStringList hostnames(HostType type);
	QList<Host> hosts(HostType type);
	bool isHost(Host host, HostType type);
	bool isHostname(QString host, HostType type);
	void addHost(Host host, HostType type);
	void addHosts(QList<Host> knownhosts, HostType type);

	// audio-related
	QAudioFormat defaultAudioFormat();
	QAudioFormat currentAudioFormat();
	QAudioFormat chooseAudioFormat(int freq, int samplesize, int channels);
	QAudioDeviceInfo defaultMicrophone();
	QAudioDeviceInfo defaultSpeaker();
	QAudioDeviceInfo currentMicrophone();
	QAudioDeviceInfo currentSpeaker();
	void setCurrentMicrophone(QAudioDeviceInfo device);
	void setCurrentSpeaker(QAudioDeviceInfo device);
	QAudioDeviceInfo getMicrophone(QString devicename);
	QAudioDeviceInfo getSpeaker(QString devicename);

	int DEFAULT_PORT;
	int SOCKET_READ_TIMEOUT;
	int SOCKET_CONNECT_TIMEOUT;
	int CONTACT_SCAN_INTERVAL;

	QString DEFAULT_CONTACT_HOSTS[];

private:
	void readHostConfig(HostType type);
	void writeHostConfig();

	// host-related
	QStringList m_localhosts;
	QList<Host> m_knownhosts;
	QList<Host> m_unknownhosts;
	QHash<HostType, bool> m_hosts_initialized;
	QHash<HostType, QMutex*> m_hosts_lock;

	// system-related
	long m_uid;
	long m_uptime;

	// audio-related
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
