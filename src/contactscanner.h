#ifndef CONTACTSCANNER_H
#define CONTACTSCANNER_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>

#include "contact.h"
#include "thread.h"
#include "id.h"

class ContactList;
class PingClient;

class ContactScanner: public QObject, public ID
{
Q_OBJECT
public:
	explicit ContactScanner(QObject* parent = 0);
	QString id() const;

signals:

public slots:
	void start();
	void scanSoon();
	void scanNow();
	void increasePriority(Host host);

	void onDisplayError(QAbstractSocket::SocketError);

private:
	QHash<Host, PingClient *> m_connections;

};

#endif // CONTACTSCANNER_H
