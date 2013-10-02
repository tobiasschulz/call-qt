/*
 * serverthread.cpp
 *
 *  Created on: 18.08.2013
 *      Author: tobias
 */

#include <QTimer>

#include "serverrequest.h"
#include "connection.h"
#include "contactlist.h"
#include "chatclient.h"
#include "chattab.h"
#include "call.h"
#include "maingui.h"
#include "config.h"

ServerRequest::ServerRequest(int socketDescriptor, Thread* thread, QObject *parent)
		: QObject(parent), socketDescriptor(socketDescriptor), m_connection(0), m_thread(thread)
{
	log.debug("constuctor()");
}

QString ServerRequest::id() const
{
	return "ServerRequest<" + (m_connection != 0 ? m_connection->id() : "?") + ">";
}
QString ServerRequest::print(PrintFormat format) const
{
	QString data((m_connection != 0 ? m_connection->print(PRINT_ONLY_DATA) : "?"));

	if (format == PRINT_ONLY_NAME)
		return "ServerRequest";
	else if (format == PRINT_ONLY_DATA)
		return data;
	else
		return "ServerRequest " + data;
}

void ServerRequest::start()
{
	log.debug("start()");
	m_connection = new Connection(Connection::SERVER, this);
	QObject::connect(m_connection.data(), &Connection::contactFound, ContactList::instance(), &ContactList::addContact);
	QObject::connect(m_connection.data(), &Connection::disconnected, m_thread.data(), &Thread::quit);
	QObject::connect(m_thread.data(), &Thread::finished, m_connection.data(), &Connection::disconnect);
	QObject::connect(m_connection.data(), &Connection::connected, this, &ServerRequest::onConnected);
	m_connection->connect(socketDescriptor);
}

void ServerRequest::onConnected()
{
	QHash<QString, QString> headers = m_connection->headers();
	QString uid = headers["uid"];
	if (uid.toInt() == Config::instance()->uid()) {
		Config::instance()->addHost(m_connection->host(), Config::LOCALHOST);
	}
	if (headers.contains("known-hosts")) {
		QList<Host> remoteKnownHosts = deserializeList<Host>(headers["known-hosts"].split(", "));
		Config::instance()->addHosts(remoteKnownHosts, Config::KNOWN_HOST);
	}
	QString request = headers["request"];
	log.debug("request = %1", request);
	if (request == "Status") {
		this->setVerbose(NONE);
		onStatusConnection();
	} else if (request == "Chat") {
		onChatConnection();
	} else if (request == "Call") {
		onCallConnection();
	}
}

void ServerRequest::onStatusConnection()
{
	log.debug("onStatusConnection()");
}

void ServerRequest::onChatConnection()
{
	log.debug("onChatConnection()");
	addChatTab();
}

void ServerRequest::onCallConnection()
{
	log.debug("onCallConnection()");
	//openChatTab();

	Call* call = Call::instance(m_connection->contact());
	m_connection->setParent(NULL);
	m_connection->moveToThread(call->thread());
	call->setConnection(m_connection);

	//QTimer::singleShot(0, Call::instance(m_connection->contact()), SLOT(open()));
}

void ServerRequest::addChatTab()
{
	QObject::connect(Main::tabs(), &Tabs::contactTabAvailable, this, &ServerRequest::onChatTabOpened);
	QObject::connect(this, &ServerRequest::addContactTab, Main::tabs(), &Tabs::addContactTab);
	emit addContactTab(m_connection->contact());
}

void ServerRequest::onChatTabOpened(Contact contact)
{
	if (contact == m_connection->contact()) {
		log.debug("onChatTabOpened: %1 (my contact)", Log::print(contact));

		ChatClient* chatclient = new ChatClient(m_connection->contact(), m_connection);
		ChatTab* chattab = ChatTab::instance(m_connection->contact());
		QObject::connect(chatclient, &ChatClient::receivedMessage, chattab, &ChatTab::onReceivedMessage);
		chatclient->connect(m_connection);

	} else {
		log.debug("onChatTabOpened: %1 (other contact)", Log::print(contact));
	}
}
