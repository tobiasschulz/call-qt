/*
 * audioinfo.cpp
 *
 *  Created on: 30.08.2013
 *      Author: tobias
 */

#include <qendian.h>
#include <cmath>
#include <limits>
using namespace std;

#include "audioinfo.h"

bool AudioInfo::DO_DEBUG = 1;
int AudioInfo::BUFFER_SIZE = 262144;

AudioInfo::AudioInfo(QIODevice* device, const QAudioFormat &format, QObject *parent)
		: QIODevice(parent), m_device(device), m_format(format), m_buffer(new char[BUFFER_SIZE]()),
			m_doUpdateLevel(false), m_doChangeVolume(true), m_maxAmplitude(0), m_baseAmplitude(0), m_level(0.0),
			m_volume(1.0), m_timer()
{
	switch (m_format.sampleSize()) {
	case 8:
		switch (m_format.sampleType()) {
		case QAudioFormat::UnSignedInt:
			m_maxAmplitude = 255;
			break;
		case QAudioFormat::SignedInt:
			m_maxAmplitude = 127;
			break;
		default:
			break;
		}
		break;
	case 16:
		switch (m_format.sampleType()) {
		case QAudioFormat::UnSignedInt:
			m_maxAmplitude = 65535;
			break;
		case QAudioFormat::SignedInt:
			m_maxAmplitude = 32767;
			break;
		default:
			break;
		}
		break;

	case 32:
		switch (m_format.sampleType()) {
		case QAudioFormat::UnSignedInt:
			m_maxAmplitude = 0xffffffff;
			break;
		case QAudioFormat::SignedInt:
			m_maxAmplitude = 0x7fffffff;
			break;
		case QAudioFormat::Float:
			m_maxAmplitude = 0x7fffffff; // Kind of
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
		m_baseAmplitude = (m_maxAmplitude + 1) / 2;
	} else {
		m_baseAmplitude = 0;
	}
}

AudioInfo::~AudioInfo()
{
	delete[] m_buffer;
}

QString AudioInfo::id() const
{
	return "AudioInfo<" + m_device->objectName() + ">";
}

void AudioInfo::start()
{
	open(QIODevice::ReadWrite);
	connect(&m_timer, &QTimer::timeout, this, &AudioInfo::update);
	m_timer.start(500);
}

void AudioInfo::stop()
{
	close();
}

void AudioInfo::update()
{
	emit levelUpdated(m_level);
	m_level = 0;
}

bool AudioInfo::isSequential() const
{
	return m_device->isSequential();
}

void AudioInfo::setUpdateLevel(bool updateLevel)
{
	m_doUpdateLevel = updateLevel;
}

void AudioInfo::setChangeVolume(bool changeVolume)
{
	m_doChangeVolume = changeVolume;
}
void AudioInfo::setVolume(qreal volume)
{
	m_volume = volume;
}

bool AudioInfo::isLevelUpdatesEnabled()
{
	return m_doUpdateLevel;
}

qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
	qint64 len = m_device->read(data, maxlen);
	if (m_doUpdateLevel && len > 0) {
		qreal level = updateLevel(data, len);
		m_level = qMax(m_level, level);
	}
	if (m_doChangeVolume && len > 0) {
		changeVolume(data, len);
	}
	return len;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
	len = qMin(len, (qint64) BUFFER_SIZE);
	memcpy(m_buffer, data, len);
	if (m_doChangeVolume) {
		changeVolume(m_buffer, len);
	}
	qint64 written = m_device->write(m_buffer, len);
	if (m_doUpdateLevel && written > 0) {
		qreal level = updateLevel(m_buffer, written);
		m_level = qMax(m_level, level);
	}
	return written;
}

qint64 AudioInfo::decode(const unsigned char *ptr)
{
	qint64 value = 0;

	if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
		value = *reinterpret_cast<const quint8*>(ptr);
	} else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt) {
		value = qAbs(*reinterpret_cast<const qint8*>(ptr));
	} else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
		if (m_format.byteOrder() == QAudioFormat::LittleEndian)
			value = qFromLittleEndian<quint16>(ptr);
		else
			value = qFromBigEndian<quint16>(ptr);
	} else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt) {
		if (m_format.byteOrder() == QAudioFormat::LittleEndian)
			value = qAbs(qFromLittleEndian<qint16>(ptr));
		else
			value = qAbs(qFromBigEndian<qint16>(ptr));
	} else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
		if (m_format.byteOrder() == QAudioFormat::LittleEndian)
			value = qFromLittleEndian<quint32>(ptr);
		else
			value = qFromBigEndian<quint32>(ptr);
	} else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt) {
		if (m_format.byteOrder() == QAudioFormat::LittleEndian)
			value = qAbs(qFromLittleEndian<qint32>(ptr));
		else
			value = qAbs(qFromBigEndian<qint32>(ptr));
	} else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float) {
		value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff); // assumes 0-1.0
	}
	if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
		// value = m_maxAmplitude - qAbs(value - m_baseAmplitude) * 2;
		//log.debug("%1 = %2 - qAbs(%3* 2 - %4)", m_maxAmplitude - qAbs(value * 2 - m_maxAmplitude), m_maxAmplitude,	value, m_maxAmplitude);
		if (value >= m_baseAmplitude) {
			value -= m_maxAmplitude;
		}
		value *= 2;
	}
	return value;
}

void AudioInfo::encode(qint64 value, unsigned char *ptr)
{
	if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
		value /= 2;
		if (value < 0) {
			value += m_maxAmplitude;
		}
	}

	if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
		*reinterpret_cast<quint8*>(ptr) = value;
	} else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt) {
		*reinterpret_cast<qint8*>(ptr) = value;
	} else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
		if (m_format.byteOrder() == QAudioFormat::LittleEndian)
			qToLittleEndian<quint16>(value, ptr);
		else
			qToBigEndian<quint16>(value, ptr);
	} else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt) {
		if (m_format.byteOrder() == QAudioFormat::LittleEndian)
			qToLittleEndian<qint16>(value, ptr);
		else
			qToBigEndian<qint16>(value, ptr);
	} else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
		if (m_format.byteOrder() == QAudioFormat::LittleEndian)
			qToLittleEndian<quint32>(value, ptr);
		else
			qToBigEndian<quint32>(value, ptr);
	} else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt) {
		if (m_format.byteOrder() == QAudioFormat::LittleEndian)
			qToLittleEndian<qint32>(value, ptr);
		else
			qToBigEndian<qint32>(value, ptr);
	} else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float) {
		// fuck you!!
	}
}

void AudioInfo::changeVolume(char *data, qint64 len)
{
	if (m_maxAmplitude) {
		Q_ASSERT(m_format.sampleSize() % 8 == 0);
		const int channelBytes = m_format.sampleSize() / 8;
		const int sampleBytes = m_format.channelCount() * channelBytes;
		Q_ASSERT(len % sampleBytes == 0);
		const int numSamples = len / sampleBytes;

		unsigned char *ptr = reinterpret_cast<unsigned char *>(data);

		for (int i = 0; i < numSamples; ++i) {
			for (int j = 0; j < m_format.channelCount(); ++j) {
				qint64 value = decode(ptr);

				value *= m_volume;

				encode(value, ptr);
				ptr += channelBytes;
			}
		}

	} else {
		log.debug("m_maxAmplitude is NULL!");
	}
}

qreal AudioInfo::updateLevel(const char* data, qint64 len)
{
	if (m_maxAmplitude) {
		Q_ASSERT(m_format.sampleSize() % 8 == 0);
		const int channelBytes = m_format.sampleSize() / 8;
		const int sampleBytes = m_format.channelCount() * channelBytes;
		Q_ASSERT(len % sampleBytes == 0);
		const int numSamples = len / sampleBytes;

		qint64 minValue = 0;
		qint64 maxValue = 0;
		qint64 avgValue = 0;
		qint64 rmsValue = 0;

		const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

		for (int i = 0; i < numSamples; ++i) {
			for (int j = 0; j < m_format.channelCount(); ++j) {
				qint64 value = decode(ptr);
				value = qAbs(value);

				minValue = qMin(value, minValue);
				maxValue = qMax(value, maxValue);
				avgValue += value;
				rmsValue += pow(value, 2);
				ptr += channelBytes;
			}
		}
		if (numSamples > 0) {
			avgValue /= m_format.channelCount() * numSamples;
			rmsValue = sqrt(rmsValue / (m_format.channelCount() * numSamples));
		}

		maxValue = qMin(maxValue, (qint64) m_maxAmplitude);
		//qreal level = qreal(avgValue) / m_maxAmplitude;
		//qreal level = maxDb;
		qreal level = (qreal) maxValue / m_maxAmplitude;

		if (DO_DEBUG) {
			log.debug("level is %1\% (max = %2, min = %3, avg = %4, rms = %5, samples = %6)\t%7", (int) (level * 100),
					maxValue, minValue, avgValue, rmsValue, m_format.channelCount() * numSamples);
		}

		return level;
	} else {
		log.debug("m_maxAmplitude is NULL!");
		return -1;
	}
}

