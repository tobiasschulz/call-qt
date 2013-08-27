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

	void debug(QString format) const;
	void debug(QString format, QVariant arg1) const;
	void debug(QString format, QVariant arg1, QVariant arg2) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5) const;
	void debug(QString format, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5,
			QVariant arg6) const;

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

	virtual QString id() const;
	virtual QString print() const;
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

#endif /* LOG_H */
