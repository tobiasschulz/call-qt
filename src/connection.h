#ifndef CONNECTION_H
#define CONNECTION_H

#include <QHostAddress>
#include <QString>
#include <QTcpSocket>
#include <QTimer>

#include "contact.h"
#include "log.h"

class Connection: public QObject, public Id {
Q_OBJECT

public:
	enum Type {
		STATUS, SERVER, CALL, CHAT, PING
	};

	Connection(Type type, QObject* parent = 0);

	void connect(QTcpSocket* socket);
	void connect(Host host);

	virtual QString id() const = 0;

	bool isConnected();

signals:
	void contactFound(Contact);
	void readyRead();
	void connected();
	void disconnected();
	void socketError(Contact, QString error);
	void socketError(QString error);
	void connectFailed(QString error);
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
	Contact m_contact;
	QHash<QString, QString>* m_headers;
	Type m_type;
	QString m_description;
	QTimer connecttimer;
	QTimer readtimer;
};

#endif // CONNECTION_H
