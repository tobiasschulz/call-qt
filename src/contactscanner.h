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
	explicit ContactScanner(ContactList* contactlist, QObject* parent = 0);
	QString id() const;

signals:

public slots:
	void onContactFound(Contact* contact);
	void onResetContacts();
	void scan();

	void onReadyRead();
	void onDisplayError(QAbstractSocket::SocketError);

private:
	StatusConnection* connect(QString hostname, quint16 port);
	void run();

	ContactList* m_contactlist;
	QHash<QString, StatusConnection *> m_connections;

};

#endif // CONTACTSCANNER_H
