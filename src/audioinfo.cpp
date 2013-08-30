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

AudioInfo::AudioInfo(QIODevice* device, const QAudioFormat &format, QObject *parent)
		: QIODevice(parent), m_device(device), m_format(format), m_maxAmplitude(0), m_level(0.0)

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
}

void AudioInfo::stop()
{
	close();
}

bool AudioInfo::isSequential() const
{
	return m_device->isSequential();
}
/*
 qint64 AudioInfo::bytesAvailable() const
 {
 return m_device->bytesAvailable() + QIODevice::bytesAvailable();
 }

 qint64 AudioInfo::bytesToWrite() const
 {
 return m_device->bytesToWrite() + QIODevice::bytesToWrite();
 }

 bool AudioInfo::canReadLine() const
 {
 return m_device->canReadLine();
 }

 QIODevice::OpenMode AudioInfo::openMode() const
 {
 return m_device->openMode();
 }
 */
qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
	qint64 len = m_device->read(data, maxlen);
	if (len > 0) {
		qreal level = updateLevel(data, len);
		if (level >= 0)
			m_level = level;
	}
	return len;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
	qint64 written = m_device->write(data, len);
	if (written > 0) {
		qreal level = updateLevel(data, len);
		if (level >= 0)
			m_level = level;
	}
	return written;
}

qreal AudioInfo::updateLevel(const char *data, qint64 len)
{
	if (m_maxAmplitude) {
		Q_ASSERT(m_format.sampleSize() % 8 == 0);
		const int channelBytes = m_format.sampleSize() / 8;
		const int sampleBytes = m_format.channelCount() * channelBytes;
		Q_ASSERT(len % sampleBytes == 0);
		const int numSamples = len / sampleBytes;

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
					//value -= m_maxAmplitude / 2;
					value = qAbs(value);
				}

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

		maxValue = qMin(maxValue, (qint64)m_maxAmplitude);
		//qreal level = qreal(avgValue) / m_maxAmplitude;
		//qreal level = maxDb;
		qreal level = (qreal)rmsValue / m_maxAmplitude;

		log.debug("level is %1 (max = %2, avg = %3, rms = %4, samples = %5)", level, maxValue, avgValue, rmsValue,
				m_format.channelCount() * numSamples);
		return level;
	} else {
		log.debug("m_maxAmplitude is NULL!");
		return -1;
	}
}

