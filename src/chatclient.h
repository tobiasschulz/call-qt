#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QMutex>
#include <QStringList>

#include "contact.h"
#include "connection.h"

class ChatClient: public QObject, public ID
{
Q_OBJECT

public:
	ChatClient(const Contact& contact, QObject* parent = 0);
	ChatClient(const Host& host, QObject* parent = 0);
	ChatClient(Connection* host, QObject* parent = 0);
	virtual QString id() const;

	void checkForErrors();

signals:
	void receivedMessage(QString msg);
	void sendMessageFailed(QString msg);

public slots:
	void connect();
	void sendMessage(QString msg);
	void flush();
	void onReadyRead();
	void onConnected();
	void onDisconnected();
	void onSocketError(QString error, Host host);
	void onConnectFailed(QString error, Host host);

private:
	Host m_host;
	Contact m_contact;
	Connection* m_connection;
	QStringList m_messagequeue;
	QMutex m_messagequeue_mutex;
};

#endif // CHATCLIENT_H
