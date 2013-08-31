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
#include "networkutil.h"

Config* Config::m_instance(0);

Config::Config(QObject *parent)
		: QObject(parent), DEFAULT_PORT(4000), SOCKET_READ_TIMEOUT(7000), SOCKET_CONNECT_TIMEOUT(2000),
			CONTACT_SCAN_INTERVAL(60000), DEFAULT_CONTACT_HOSTS(), m_localhosts(), m_uid(0),
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
	QStringList list;
	if (1)
		list << "127.0.0.1" << "dsl-ka.tobias-schulz.eu" << "192.168.224.3" << "192.168.224.5" << "192.168.224.150";
	else
		list << "127.0.0.1" << "192.168.223.3" << "192.168.223.5" << "192.168.223.7" << "192.168.223.9"
				<< "192.168.223.150" << "192.168.223.151" << "192.168.223.152" << "192.168.223.153" << "192.168.223.154"
				<< "192.168.224.3" << "192.168.224.5" << "192.168.224.7" << "192.168.224.9" << "192.168.224.150"
				<< "192.168.224.151" << "192.168.224.152" << "192.168.224.153" << "192.168.224.154" << "192.168.25.100"
				<< "192.168.25.101" << "192.168.25.102" << "192.168.25.103" << "dsl-ka.tobias-schulz.eu"
				<< "dsl-hg.tobias-schulz.eu" << "freehal.net";
	return list;
}

QList<Host> Config::defaultHosts()
{
	QList<Host> hosts;
	for (int i = 0; i <= 5; ++i) {
		foreach (const QString & hostname, defaultHostnames())
		{
			//QHostAddress hostaddr = NetworkUtil::parseHostname(hostname);
			//hosts << Host(hostaddr, Config::DEFAULT_PORT + i * 10);
			hosts << Host(hostname, Config::DEFAULT_PORT + i * 10);
		}
	}
	return hosts;
}

void Config::addLocalhost(Host host)
{
	readConfigLocalhosts();
	if (!m_localhosts.contains(host.hostname()))
		m_localhosts << host.hostname();
	if (!m_localhosts.contains(host.address().toString()))
		m_localhosts << host.address().toString();
	m_localhosts.removeAll(Host::INVALID_HOST.hostname());
	writeConfigLocalhosts();
}
QStringList Config::localhosts()
{
	if (m_localhosts.size() == 0) {
		readConfigLocalhosts();
	}
	return m_localhosts;
}
bool Config::isLocalhost(QString host)
{
	return m_localhosts.contains(host);
}
void Config::readConfigLocalhosts()
{
	QSettings settings;
	m_localhosts = settings.value("contacts/localhosts", QStringList()).toStringList();
}
void Config::writeConfigLocalhosts()
{
	QSettings settings;
	settings.setValue("contacts/localhosts", m_localhosts);
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

QIcon Config::icon(QString iconname)
{
	return QIcon("img/" + iconname + ".png");
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

