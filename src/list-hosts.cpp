/*
 * list-hosts.cpp
 *
 *  Created on: 10.10.2013
 *      Author: tobias
 */

#include <QSettings>

#include "list-hosts.h"
#include "config.h"
#include "dnscache.h"

using namespace List;

QMutex Hosts::m_lock;

Hosts::Hosts(QObject *parent)
		: QObject(parent), m_hoststate()
{
}

Hosts& Hosts::instance()
{
	static Hosts instance;
	return instance;
}

QString Hosts::id() const
{
	return "List::Hosts";
}

void Hosts::setHostOnline(Host host)
{
	if (!m_hoststate[host].contains(HOST_ONLINE)) {
		m_hoststate[host] << HOST_ONLINE;
		emit hostOnline(host);
	}
}

void Hosts::setHostOffline(Host host)
{
	if (m_hoststate[host].contains(HOST_ONLINE)) {
		m_hoststate[host].remove(HOST_ONLINE);
		emit hostOffline(host);
	}
}

bool Hosts::isHostOnline(Host host)
{
	return m_hoststate[host].contains(HOST_ONLINE);
}

void Hosts::addSignals(Connection* connection)
{
	QObject::connect(connection, &Connection::hostOnline, this, &Hosts::setHostOnline);
	QObject::connect(connection, &Connection::hostOffline, this, &Hosts::setHostOffline);
}

void Hosts::addHostState(Host host, HostState state)
{
	if (!m_hoststate[host].contains(state)) {
		//log.debug("addHostState: %1, %2", Log::print(host), state);
		m_hoststate[host] << state;
		emit hostStateChanged(host);
	}
}

void Hosts::removeHostState(Host host, HostState state)
{
	if (m_hoststate[host].contains(state)) {
		//log.debug("removeHostState: %1, %2", Log::print(host), state);
		m_hoststate[host].remove(state);
		emit hostStateChanged(host);
	}
}

Hosts::HostStateSet Hosts::hostState(Host host)
{
	return m_hoststate[host];
}

Hosts::HostStateSet Hosts::hostState(QString hostname)
{
	HostStateSet state;
	foreach (const Host& host, m_hoststate.keys())
	{
		if (host.displaynames().contains(hostname)) {
			state.unite(m_hoststate[host]);
		}
	}
	return state;
}

Hosts::HostStateSet Hosts::hostState(QList<Host> hosts)
{
	HostStateSet state;
	foreach (const Host& host, hosts)
	{
		if (m_hoststate.contains(host)) {
			state.unite(m_hoststate[host]);
		}
	}
	return state;
}
