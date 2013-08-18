#ifndef CONNECTION_H
#define CONNECTION_H

#include <QHostAddress>
#include <QString>
#include <QTcpSocket>

class Connection: public QObject {
Q_OBJECT
public:
	Connection(QObject* parent, QTcpSocket* socket);
	Connection(QObject* parent, Contact contact);
	Connection(QObject* parent, QHostAddress host, quint16 port);

	void setSocket(QTcpSocket* socket);

signals:
	void readyRead();
	void connected();
	void error(QAbstractSocket::SocketError);

public slots:
	void onReadyRead();
	void onConnected();
	void onError(QAbstractSocket::SocketError);

protected:
	QTcpSocket* m_socket;
	Contact m_contact;

};

#endif // CONNECTION_H
