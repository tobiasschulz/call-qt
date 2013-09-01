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
#include <QTimer>

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
	qreal volume() const
	{
		return m_volume;
	}
	bool isSequential() const;
	bool isLevelUpdatesEnabled();

	static bool DO_DEBUG;
	static int BUFFER_SIZE;

protected:
	qint64 readData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);
	qreal updateLevel(const char *data, qint64 len);
	void changeVolume(char *data, qint64 len);

signals:
	void levelUpdated(qreal level);

public slots:
	void setUpdateLevel(bool doLevelUpdates);
	void setVolume(qreal volume);
	void setChangeVolume(bool changeVolume);

private:
	void update();
	qint64 decode(const unsigned char *ptr);
	void encode(qint64 value, unsigned char *ptr);

	QPointer<QIODevice> m_device;
	const QAudioFormat m_format;
	char* m_buffer;
	bool m_doUpdateLevel;
	bool m_doChangeVolume;
	qint32 m_maxAmplitude;
	qint32 m_baseAmplitude;
	qreal m_level; // 0.0 <= m_level <= 1.0
	qreal m_volume; // 0.0 <= m_volume <= 2.0
	QTimer m_timer;
};

#endif /* AUDIOINFO_H */
