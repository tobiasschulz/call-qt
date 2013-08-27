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
#include "id.h"

class Config: public QObject
{
Q_OBJECT

public:
	static QStringList localhosts();
	static QStringList defaultHostnames();
	static QList<Host> defaultHosts();
	static long uid();
	static long uptime();
	static QIcon icon(QString iconname);
	static QString version();
	static QString build();
	static void addLocalhost(Host host);
	static bool isLocalhost(QString host);

	static int DEFAULT_PORT;
	static int SOCKET_READ_TIMEOUT;
	static int SOCKET_CONNECT_TIMEOUT;
	static int CONTACT_SCAN_INTERVAL;

	static QString DEFAULT_CONTACT_HOSTS[];

private:
	static void readConfigLocalhosts();
	static void writeConfigLocalhosts();

	static QStringList m_localhosts;
	static long m_uid;
	static long m_uptime;
};

#endif /* CONFIG_H */
