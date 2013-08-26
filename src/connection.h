#ifndef CONNECTION_H
#define CONNECTION_H

#include <QHostAddress>
#include <QString>
#include <QTcpSocket>
#include <QTimer>

#include "contact.h"
#include "log.h"

class Connection: public QObject, public Id
{
Q_OBJECT

public:
	enum Type
	{
		STATUS, SERVER, CALL, CHAT, PING
	};

	Connection(Type type, QObject* parent = 0);

	void connect(QTcpSocket* socket);
	void connect(Host host);

	virtual QString id() const;

	bool isConnected() const;
	Host host() const;
	Contact contact() const;

signals:
	void contactFound(Contact);
	void hostOnline(Host);
	void hostOffline(Host);
	void readyRead();
	void connected();
	void disconnected();
	void socketError(QString error, Host host = Contact::INVALID_HOST);
	void connectFailed(QString error, Host host = Contact::INVALID_HOST);
	void close();

public slots:
	void onReadyRead();
	void onConnected();
	void onDisconnected();
	void onError(QAbstractSocket::SocketError);
	void onConnectTimeout();
	void onReadTimeout();

protected:
	void setSocket(QTcpSocket* socket);

	QTcpSocket* m_socket;
	Host m_host;
	Contact m_contact;
	QHash<QString, QString>* m_headers;
	Type m_type;
	QString m_description;
	QTimer m_connecttimer;
	QTimer m_readtimer;
};

#endif // CONNECTION_H
