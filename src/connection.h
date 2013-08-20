#ifndef CONNECTION_H
#define CONNECTION_H

#include <QHostAddress>
#include <QString>
#include <QTcpSocket>

#include "contact.h"
#include "log.h"

class Connection: public QObject, public Id {
Q_OBJECT

public:
	enum Type {
		STATUS, SERVER, CALL, CHAT, PING
	};

	Connection(QTcpSocket* socket, Type type, QObject* parent = 0);
	Connection(Contact* contact, Type type, QObject* parent = 0);
	Connection(QHostAddress hostaddr, quint16 port, Type type, QObject* parent = 0);

	void connect(QTcpSocket* socket);
	void connect(Contact* contact);
	void connect(QHostAddress hostaddr, quint16 port);

	virtual QString id() const = 0;

	bool isConnected();

signals:
	void contactFound(Contact*);
	void readyRead();
	void connected();
	void disconnected();
	void error(QAbstractSocket::SocketError);

public slots:
	void onReadyRead();
	void onConnected();
	void onDisconnected();
	void onError(QAbstractSocket::SocketError);

protected:
	void setSocket(QTcpSocket* socket);

	QTcpSocket* m_socket;
	Contact* m_contact;
	QHash<QString, QString>* m_headers;
	Type m_type;
	QString m_description;
};

#endif // CONNECTION_H
