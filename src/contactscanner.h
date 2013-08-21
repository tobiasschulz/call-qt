#ifndef CONTACTSCANNER_H
#define CONTACTSCANNER_H

#include <QThread>
#include <QTcpSocket>

#include "contact.h"
#include "log.h"

class ContactList;
class StatusConnection;

class ContactScanner: public QThread, public Id {
Q_OBJECT
public:
	explicit ContactScanner(QObject* parent = 0);
	QString id() const;

signals:

public slots:
	void scanSoon();
	void scanNow();

	void onDisplayError(QAbstractSocket::SocketError);

private:
	StatusConnection* connect(QString hostname, quint16 port);
	void run();

	QHash<QString, StatusConnection *> m_connections;

};

#endif // CONTACTSCANNER_H
