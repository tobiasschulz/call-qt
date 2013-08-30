/*
 * audioinfo.h
 *
 *  Created on: 30.08.2013
 *      Author: tobias
 */

#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <QAudio>
#include <QAudioInput>
#include <QAudioOutput>
#include <QPointer>

#include "id.h"

class AudioInfo: public QIODevice, public ID
{
Q_OBJECT

public:
	AudioInfo(QIODevice* device, const QAudioFormat &format, QObject *parent);
	~AudioInfo();
	QString id() const;

	void start();
	void stop();

	qreal level() const
	{
		return m_level;
	}
	bool isSequential() const;
	/*qint64 bytesAvailable() const;
	qint64 bytesToWrite() const;
	bool canReadLine() const;
	OpenMode openMode() const;*/

protected:
	qint64 readData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);
	qreal updateLevel(const char *data, qint64 len);

signals:

private:
	QPointer<QIODevice> m_device;
	const QAudioFormat m_format;
	quint32 m_maxAmplitude;
	qreal m_level; // 0.0 <= m_level <= 1.0
};

#endif /* AUDIOINFO_H */
