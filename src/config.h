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

class Config: public QObject {
Q_OBJECT

public:
	static QStringList hosts_to_contact();
	static long uid();
	static long uptime();

	static int DEFAULT_PORT;
	static int SOCKET_READ_TIMEOUT;
	static int SOCKET_CONNECT_TIMEOUT;

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
