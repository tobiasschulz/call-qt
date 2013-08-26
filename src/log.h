/*
 * log.h
 *
 *  Created on: 19.08.2013
 *      Author: tobias
 */

#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QTcpSocket>

#define Q(string) (qPrintable(string))

class Log;
class Id;

class Log: public QObject
{
Q_OBJECT

public:
	Log(Id* id);
	//Log(Id* id, QObject* parent);

	static QString print(QTcpSocket* socket);
	static QString print(Id* id);
	static QString print(const Id& id);

	void debug(QString format) const;
	void debug(QString format, QVariant arg1) const;
	void debug(QString format, QVariant arg1, QVariant arg2) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4,
			QVariant arg5) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
			QVariant arg6) const;

private:
	Id* m_id;
};

class Id
{
public:
	Id();
	virtual ~Id()
	{
	}

	virtual QString id() const;
	virtual const Id& fromId(QString) const;

protected:
	Log log;
};

class StaticId: public Id
{
public:
	StaticId(QString id);
	QString id() const;

private:
	QString m_id;
};

class InvalidId: public Id
{
public:
	InvalidId();
	QString id() const;
};

uint qHash(const Id& c);

#endif /* LOG_H */
