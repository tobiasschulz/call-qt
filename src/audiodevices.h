#ifndef AUDIODEVICES_H
#define AUDIODEVICES_H

#include <QWidget>
#include <QString>

#include "tab.h"

namespace Ui
{
class AudioDevices;
}

class AudioDevices: public Tab
{
Q_OBJECT

public:
	explicit AudioDevices(QWidget *parent = 0);
	~AudioDevices();

	QString id() const;

private:
	Ui::AudioDevices *ui;
};

#endif // AUDIODEVICES_H
