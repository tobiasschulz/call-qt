#include <QComboBox>

#include "audiodevices.h"
#include "ui_audiodevices.h"
#include "systemutil.h"
#include "config.h"

AudioDevices::AudioDevices()
		: Tab("Audio Devices", Config::instance()->icon("microphone")), ui(new Ui::AudioDevices)
{

	ui->setupUi(this);
	QObject::connect(this, SIGNAL(focus()), this, SLOT(setFocus()));

	updateDevices();

	QObject::connect(ui->comboboxMicrophones, SIGNAL(activated(int)), this, SLOT(microphoneChanged(int)));
	QObject::connect(ui->comboboxSpeakers, SIGNAL(activated(int)), this, SLOT(speakerChanged(int)));
}

AudioDevices::~AudioDevices()
{
	delete ui;
}

QString AudioDevices::id() const
{
	return "AudioDevices";
}

void AudioDevices::updateDevices()
{
	// microphones
	fillCombobox(ui->comboboxMicrophones, QAudioDeviceInfo::availableDevices(QAudio::AudioInput),
			Config::instance()->currentMicrophone());
	// speakers
	fillCombobox(ui->comboboxSpeakers, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput),
			Config::instance()->currentSpeaker());
}

void AudioDevices::fillCombobox(QComboBox* combobox, QList<QAudioDeviceInfo> devices, QAudioDeviceInfo currentDevice)
{
	QMutexLocker locker(&m_deviceslock);
	combobox->clear();
	int currentIndex = -1;
	for (int i = 0; i < devices.size(); ++i) {
		QString name = devices.at(i).deviceName();
		combobox->addItem(name, qVariantFromValue(devices.at(i)));
		if (name == currentDevice.deviceName()) {
			currentIndex = i;
		}
	}
	if (currentIndex != -1) {
		combobox->setCurrentIndex(currentIndex);
	}
}

void AudioDevices::microphoneChanged(int index)
{
	QMutexLocker locker(&m_deviceslock);
	if (index >= 0 && index < ui->comboboxMicrophones->count()) {
		QAudioDeviceInfo device = ui->comboboxMicrophones->itemData(index).value<QAudioDeviceInfo>();
		Config::instance()->setCurrentMicrophone(device);
	}
}

void AudioDevices::speakerChanged(int index)
{
	QMutexLocker locker(&m_deviceslock);
	if (index >= 0 && index < ui->comboboxSpeakers->count()) {
		QAudioDeviceInfo device = ui->comboboxSpeakers->itemData(index).value<QAudioDeviceInfo>();
		Config::instance()->setCurrentSpeaker(device);
	}
}

