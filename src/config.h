/*
 * config.h
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QIcon>

#include "contact.h"

class Config: public QObject
{
Q_OBJECT

public:
	static QStringList hostnames_to_contact();
	static QList<Host> hosts_to_contact();
	static long uid();
	static long uptime();
	static QIcon icon(QString iconname);
	static QString version();
	static QString build();

	static int DEFAULT_PORT;
	static int SOCKET_READ_TIMEOUT;
	static int SOCKET_CONNECT_TIMEOUT;
	static int CONTACT_SCAN_INTERVAL;

	static QString DEFAULT_CONTACT_HOSTS[];

private:
	static long m_uid;
	static long m_uptime;
};

#ifdef __CDT_PARSER__
#undef foreach
#define foreach(a, b) for(a; ; )
#endif

#endif /* CONFIG_H */
