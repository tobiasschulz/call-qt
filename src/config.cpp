/*
 * config.cpp
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#include <QDateTime>

#include "config.h"

int Config::DEFAULT_PORT = 4000;
int Config::SOCKET_READ_TIMEOUT = 7000;
int Config::SOCKET_CONNECT_TIMEOUT = 2000;
long Config::m_uid = 0;
long Config::m_uptime = QDateTime::currentMSecsSinceEpoch();

QStringList Config::hosts_to_contact() {
	QStringList list;
	list << "127.0.0.1";
	if (0)
		list << "192.168.223.3" << "192.168.223.5" << "192.168.223.7" << "192.168.223.9" << "192.168.223.150"
				<< "192.168.223.151" << "192.168.223.152" << "192.168.223.153" << "192.168.223.154" << "192.168.224.3"
				<< "192.168.224.5" << "192.168.224.7" << "192.168.224.9" << "192.168.224.150" << "192.168.224.151"
				<< "192.168.224.152" << "192.168.224.153" << "192.168.224.154" << "192.168.25.100" << "192.168.25.101"
				<< "192.168.25.102" << "192.168.25.103" << "dsl-ka.tobias-schulz.eu" << "dsl-hg.tobias-schulz.eu"
				<< "freehal.net";
	return list;
}

long Config::uid() {
	if (m_uid == 0) {
		qsrand(QDateTime::currentMSecsSinceEpoch());
		m_uid = qrand();
		return m_uid;
	} else {
		return m_uid;
	}
}

long Config::uptime() {
	return m_uptime;
}

QIcon Config::icon(QString iconname) {
	return QIcon("img/" + iconname + ".png");
}
