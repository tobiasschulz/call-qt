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

class Config: public QObject {
Q_OBJECT

public:
	static QStringList hosts_to_contact();

	static int DEFAULT_PORT;
	static int SOCKET_READ_TIMEOUT;
	static int SOCKET_CONNECT_TIMEOUT;

	static QString DEFAULT_CONTACT_HOSTS[];
};

#ifdef __CDT_PARSER__
    #undef foreach
    #define foreach(a, b) for(a; ; )
#endif

#endif /* CONFIG_H */
