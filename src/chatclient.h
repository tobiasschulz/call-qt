#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QMutex>
#include <QStringList>

#include "contact.h"
#include "connection.h"

class ChatClient: public QObject, public Id
{
Q_OBJECT

public:
	ChatClient(const Contact& contact, QObject* parent = 0);
	ChatClient(const Host& host, QObject* parent = 0);
	ChatClient(const Connection& host, QObject* parent = 0);
	virtual QString id() const;

signals:

public slots:
	void connect();
	void sendMessage(QString msg);
	void sync();
	void onConnected();

private:
	Host m_host;
	Contact m_contact;
	Connection* m_connection;
	QStringList messagequeue;
	QMutex m_messagequeue_mutex;
};

#endif // CHATCLIENT_H
