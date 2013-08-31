#include "audiodevices.h"
#include "ui_audiodevices.h"
#include "systemutil.h"
#include "config.h"

AudioDevices::AudioDevices(QWidget *parent)
		: Tab("Audio Devices", Config::instance()->icon("microphone")), ui(new Ui::AudioDevices)
{

	ui->setupUi(this);
	QObject::connect(this, SIGNAL(focus()), this, SLOT(setFocus()));
}

AudioDevices::~AudioDevices()
{
	delete ui;
}

QString AudioDevices::id() const
{
	return "AudioDevices";
}
