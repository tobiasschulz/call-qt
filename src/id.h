/*
 * id.h
 *
 *  Created on: 19.08.2013
 *      Author: tobias
 */

#ifndef ID_H
#define ID_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QTcpSocket>
#include <QAudioDeviceInfo>
#include <QAudioFormat>

#define Q(string) (qPrintable(string))

#ifdef __CDT_PARSER__
#undef foreach
#define foreach(a, b) for(a; ; )
#endif

class Log;
class ID;

class Log: public QObject
{
Q_OBJECT

public:
	Log(ID* id);
	//Log(Id* id, QObject* parent);

	static QString print(QTcpSocket* socket);
	static QString print(ID* id);
	static QString print(const ID& id);
	static QString print(QAudioDeviceInfo device);
	static QString print(QAudioFormat format);

	void debug(QString format) const;
	void debug(QString format, QVariant arg1) const;
	void debug(QString format, QVariant arg1, QVariant arg2) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
			QVariant arg6) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5, QVariant arg6,
			QVariant arg7) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5, QVariant arg6,
			QVariant arg7, QVariant arg8) const;

private:
	ID* m_id;
};

class ID
{
public:
	ID();
	virtual ~ID()
	{
	}

	enum PrintFormat
	{
		PRINT_ONLY_NAME, PRINT_ONLY_DATA, PRINT_NAME_AND_DATA
	};
	virtual QString id() const;
	virtual QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;
	virtual QString serialize() const;
	const Log& logger() const;

protected:
	Log log;
};

class StaticID: public ID
{
public:
	StaticID(QString id);
	QString id() const;

private:
	QString m_id;
};

class InvalidID: public ID
{
public:
	InvalidID();
	QString id() const;
};

uint qHash(const ID& c);

template<typename T>
QStringList serializeList(QList<T> list)
{
	QStringList list2;
	foreach (T obj, list)
	{
		list2.append(obj.serialize());
	}
	return list2;
}

template<typename T>
QList<T> deserializeList(QStringList list)
{
	QList<T> list2;
	foreach (QString str, list)
	{
		T obj;
		fromId(str, obj);
		list2.append(obj);
	}
	return list2;
}

#endif /* ID_H */
