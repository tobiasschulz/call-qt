#ifndef CONTACTSCANNER_H
#define CONTACTSCANNER_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>

#include "contact.h"
#include "log.h"

class ContactList;
class PingClient;

class ContactScanner: public QThread, public Id
{
Q_OBJECT
public:
	explicit ContactScanner(QObject* parent = 0);
	QString id() const;

signals:

public slots:
	void scanSoon();
	void scanNow();
	void increasePriority(Host host);

	void onDisplayError(QAbstractSocket::SocketError);

private:
	void run();

	QHash<Host, PingClient *> m_connections;
	QList<Host> m_unknownhosts;
	QList<Host> m_knownhosts;
	QMutex m_hosts_mutex;

};

#endif // CONTACTSCANNER_H
