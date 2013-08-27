#ifndef CONNECTION_H
#define CONNECTION_H

#include <QHostAddress>
#include <QString>
#include <QTcpSocket>
#include <QTimer>

#include "contact.h"
#include "log.h"

class Connection: public QObject, public ID
{
Q_OBJECT

public:
	enum Type
	{
		STATUS, SERVER, CALL, CHAT, PING
	};

	enum State
	{
		CLOSED, CONNECTED, CONNECTING
	};

	Connection(Type type, QObject* parent = 0);

	void connect(QTcpSocket* socket);
	void connect(Host host);

	virtual QString id() const;
	virtual QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;

	bool isConnected() const;
	Host host() const;
	Contact contact() const;
	QTcpSocket* socket() const;
	QHash<QString, QString> headers();

signals:
	void contactFound(Contact);
	void hostOnline(Host);
	void hostOffline(Host);
	void readyRead();
	void connected();
	void disconnected();
	void socketError(QString error, Host host = Host::INVALID_HOST);
	void connectFailed(QString error, Host host = Host::INVALID_HOST);
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
	State m_state;
	QHash<QString, QString> m_headers;
	Type m_type;
	QTimer m_connecttimer;
	QTimer m_readtimer;
};

#endif // CONNECTION_H
