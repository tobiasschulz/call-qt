#ifndef AUDIODEVICES_H
#define AUDIODEVICES_H

#include <QWidget>
#include <QComboBox>
#include <QString>
#include <QMutexLocker>
#include <QMutex>

#include "tab.h"

namespace Ui
{
class AudioDevices;
}

class AudioDevices: public Tab
{
Q_OBJECT

public:
	explicit AudioDevices();
	~AudioDevices();
	QString id() const;

	void updateDevices();
	void fillCombobox(QComboBox* combobox, QList<QAudioDeviceInfo> devices);

signals:

public slots:
	void microphoneChanged(int index);
	void speakerChanged(int index);

private:
	Ui::AudioDevices *ui;
	QMutex m_deviceslock;
};

#endif // AUDIODEVICES_H
