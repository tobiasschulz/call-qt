/*
 * config.cpp
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#include <QDateTime>
#include <QSettings>
#include <QMutex>
#include <QAudioDeviceInfo>

#include "config.h"
#include "dnscache.h"
#include "networkutil.h"

Config* Config::m_instance(0);

Config::Config(QObject *parent)
		: QObject(parent), DEFAULT_PORT(4000), SOCKET_READ_TIMEOUT(8000), SOCKET_CONNECT_TIMEOUT(4000),
			CONTACT_SCAN_INTERVAL(90000), DEFAULT_CONTACT_HOSTS(), m_hosts(), m_hosts_initialized(), m_uid(0),
			m_uptime(QDateTime::currentMSecsSinceEpoch()), m_audioinputdevice(), m_audiooutputdevice()
{
	m_audioinputformat = defaultAudioFormat();

	QSettings settings;
	QString microphone = settings.value("microphones/default", "").toString();
	setCurrentMicrophone(getMicrophone(microphone));
	QString speaker = settings.value("speakers/default", "").toString();
	setCurrentSpeaker(getSpeaker(speaker));
}

QString Config::id() const
{
	return "Config";
}

Config* Config::instance()
{
	static QMutex mutex;
	if (!m_instance) {
		mutex.lock();
		if (!m_instance)
			m_instance = new Config;
		mutex.unlock();
	}
	return m_instance;
}

QStringList Config::defaultHostnames()
{
	QStringList addresses;
	if (0)
		addresses << "127.0.0.1" << "dsl-ka.tobias-schulz.eu" << "192.168.224.3" << "192.168.224.5"
				<< "192.168.224.150";
	else
		addresses << "127.0.0.1" << "192.168.224.3" << "192.168.224.5" << "192.168.224.7" << "192.168.224.9"
				<< "192.168.224.150" << "192.168.224.151" << "192.168.224.152" << "192.168.224.153" << "192.168.224.154"
				<< "192.168.25.100" << "192.168.25.101" << "192.168.25.102" << "192.168.25.103"
				<< "dsl-ka.tobias-schulz.eu" << "dsl-hg.tobias-schulz.eu" << "freehal.net";
	return addresses;
}

QList<quint16> Config::defaultPorts()
{
	QList<quint16> ports;
	for (int i = 0; i <= 5; ++i) {
		ports << Config::DEFAULT_PORT + i * 10;
	}
	return ports;
}

QList<Host> Config::defaultHosts()
{
	QList<quint16> ports = defaultPorts();
	QList<Host> hosts;
	foreach (const QString & hostname, defaultHostnames())
	{
		foreach (quint16 port, ports)
		{
			hosts << Host(hostname, port);
		}
	}
	return hosts;
}

void Config::readHostConfig(HostType type)
{
	if (!m_hosts_initialized[type]) {
		m_hosts_initialized[type] = true;

		QSettings settings;

		// local
		if (type == LOCALHOST) {
			m_hosts[LOCALHOST] = deserializeList<Host>(
					settings.value("contacts/localhosts", QStringList()).toStringList());
			m_hosts[LOCALHOST].removeAll(Host::INVALID_HOST);
		}

		// known and unknown
		if (type == KNOWN_HOST || type == UNKNOWN_HOST) {
			m_hosts[UNKNOWN_HOST] << Config::instance()->defaultHosts();
			m_hosts[KNOWN_HOST] = deserializeList<Host>(
					settings.value("contacts/known-hosts", QStringList()).toStringList());
			m_hosts[KNOWN_HOST].removeAll(Host::INVALID_HOST);
			foreach (const Host& host, m_hosts[KNOWN_HOST])
			{
				log.debug("known host: %1", Log::print(host));
				if (host.isUnreachable() || host.isDynamicIP()) {
					m_hosts[KNOWN_HOST].removeAll(host);
				}
				m_hosts[UNKNOWN_HOST].removeAll(host);
			}
		}
	}
}
void Config::writeHostConfig()
{
	// log.debug("%1", "2");
	static QMutex lock;
	QMutexLocker locker(&lock);

	QSettings settings;
	settings.setValue("contacts/localhosts", serializeList(m_hosts[LOCALHOST]));

	m_hosts[KNOWN_HOST].removeAll(Host::INVALID_HOST);
	settings.setValue("contacts/known-hosts", serializeList(m_hosts[KNOWN_HOST]));
}

QStringList Config::hostnames(HostType type)
{
	// log.debug("%1", "3");
	readHostConfig(type);

	QStringList hostnames;
	foreach (const Host & host, m_hosts[type])
	{
		if (!hostnames.contains(host.displayname())) {
			hostnames << host.displayname();
		}
	}
	return hostnames;
}
QList<Host> Config::hosts(HostType type)
{
	// log.debug("%1", "4");
	readHostConfig(type);
	return m_hosts[type];
}
bool Config::isHost(Host host, HostType type)
{
	// log.debug("%1", "5");
	readHostConfig(type);
	return m_hosts[type].contains(host);
}
void Config::addHost(Host host, HostType type)
{
	// log.debug("%1", "7");
	static QMutex lock;
	QMutexLocker locker(&lock);

	if (host == Host::INVALID_HOST) {
		// ignore
	} else if (type == LOCALHOST) {
		readHostConfig(LOCALHOST);
		if (!m_hosts[LOCALHOST].contains(host)) {
			m_hosts[LOCALHOST] << host;
			writeHostConfig();
		}
	} else if (type == KNOWN_HOST) {
		readHostConfig(KNOWN_HOST);
		readHostConfig(UNKNOWN_HOST);
		m_hosts[UNKNOWN_HOST].removeAll(host);
		if (!m_hosts[KNOWN_HOST].contains(host) && host.isReachable()) {
			m_hosts[KNOWN_HOST].prepend(host);
			writeHostConfig();
		}
	} else if (type == UNKNOWN_HOST) {
		readHostConfig(KNOWN_HOST);
		readHostConfig(UNKNOWN_HOST);
		m_hosts[KNOWN_HOST].removeAll(host);
		if (!m_hosts[UNKNOWN_HOST].contains(host) && host.isReachable()) {
			m_hosts[UNKNOWN_HOST].prepend(host);
			writeHostConfig();
		}
	}
}
void Config::addHosts(QList<Host> hosts, HostType type)
{
	// log.debug("%1", "8");
	foreach (const Host& host, hosts)
	{
		addHost(host, type);
	}
}

long Config::uid()
{
	if (m_uid == 0) {
		qsrand(QDateTime::currentMSecsSinceEpoch());
		m_uid = qrand();
		return m_uid;
	} else {
		return m_uid;
	}
}

long Config::uptime()
{
	return m_uptime;
}

QIcon Config::icon(QString iconname, QString ext)
{
	return QIcon("img/" + iconname + "." + ext);
}

QMovie* Config::movie(QString moviename, QString ext)
{
	static QHash<QString, QMovie*> cache;
	if (!cache.contains(moviename)) {
		QMovie* movie = new QMovie("img/" + moviename + "." + ext);
		movie->start();
		cache[moviename] = movie;
	}
	return cache[moviename];
}

QAudioFormat Config::defaultAudioFormat()
{
	return chooseAudioFormat(44100, 16, 1);
}

QAudioFormat Config::currentAudioFormat()
{
	return m_audioinputformat;
}

QAudioFormat Config::chooseAudioFormat(int freq, int samplesize, int channels)
{
	QAudioFormat format;
	format.setSampleRate(freq);
	format.setSampleSize(samplesize);
	format.setChannelCount(channels);
	format.setSampleType(QAudioFormat::UnSignedInt);
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setCodec("audio/pcm");

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
	if (!info.isFormatSupported(format)) {
		log.debug("format not supported: %1", Log::print(format));
		format = info.nearestFormat(format);
		log.debug("trying to use nearest: %1", Log::print(format));
	}
	return format;
}

QAudioDeviceInfo Config::currentMicrophone()
{
	return m_audioinputdevice;
}

QAudioDeviceInfo Config::currentSpeaker()
{
	return m_audiooutputdevice;
}

QAudioDeviceInfo Config::defaultMicrophone()
{
	return QAudioDeviceInfo::defaultInputDevice();
}

QAudioDeviceInfo Config::defaultSpeaker()
{
	return QAudioDeviceInfo::defaultOutputDevice();
}

void Config::setCurrentMicrophone(QAudioDeviceInfo device)
{
	m_audioinputdevice = device;
	QSettings settings;
	settings.setValue("microphones/default", device.deviceName());
}

void Config::setCurrentSpeaker(QAudioDeviceInfo device)
{
	m_audiooutputdevice = device;
	QSettings settings;
	settings.setValue("speakers/default", device.deviceName());
}

QAudioDeviceInfo Config::getMicrophone(QString devicename)
{
	QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
	for (int i = 0; i < devices.size(); ++i) {
		if (devicename == devices.at(i).deviceName()) {
			return devices.at(i);
		}
	}
	return QAudioDeviceInfo::defaultInputDevice();
}

QAudioDeviceInfo Config::getSpeaker(QString devicename)
{
	QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for (int i = 0; i < devices.size(); ++i) {
		if (devicename == devices.at(i).deviceName()) {
			return devices.at(i);
		}
	}
	return QAudioDeviceInfo::defaultOutputDevice();
}
