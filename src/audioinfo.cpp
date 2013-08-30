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

bool AudioInfo::DO_DEBUG = 0;

AudioInfo::AudioInfo(QIODevice* device, const QAudioFormat &format, QObject *parent)
		: QIODevice(parent), m_device(device), m_format(format), m_doLevelUpdates(false), m_maxAmplitude(0),
			m_baseAmplitude(0), m_level(0.0), m_timer()

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
}

bool AudioInfo::isSequential() const
{
	return m_device->isSequential();
}

void AudioInfo::setLevelUpdates(bool doLevelUpdates)
{
	m_doLevelUpdates = doLevelUpdates;
}

bool AudioInfo::isLevelUpdatesEnabled()
{
	return m_doLevelUpdates;
}

qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
	qint64 len = m_device->read(data, maxlen);
	if (m_doLevelUpdates && len > 0) {
		qreal level = updateLevel(data, len);
		if (level >= 0)
			m_level = level;
	}
	return len;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
	qint64 written = m_device->write(data, len);
	if (m_doLevelUpdates && written > 0) {
		qreal level = updateLevel(data, len);
		if (level >= 0)
			m_level = level;
	}
	return written;
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
		qreal maxDb = 0;

		const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

		for (int i = 0; i < numSamples; ++i) {
			for (int j = 0; j < m_format.channelCount(); ++j) {
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
					//value /= 2;

					if (value > m_baseAmplitude)
						value = m_baseAmplitude - value;
					else
						value = value - m_baseAmplitude;
					value = qAbs(value) * 2;
					value = m_maxAmplitude - value;
				}

				minValue = qMin(value, minValue);
				maxValue = qMax(value, maxValue);
				avgValue += value;
				rmsValue += pow(value, 2);
				qreal Db = abs(20 * log10(abs(value) / m_maxAmplitude));
				if (Db < std::numeric_limits<qreal>::max())
					maxDb = qMax(maxDb, Db);
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
		qreal level = (qreal) rmsValue / m_maxAmplitude;

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

