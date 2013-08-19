#ifndef CONTACTSCANNER_H
#define CONTACTSCANNER_H

#include <QThread>
#include <QTcpSocket>

#include "log.h"

class ContactList;
class StatusConnection;

class ContactScanner: public QThread, public Id {
Q_OBJECT
public:
	explicit ContactScanner(ContactList* contacts, QObject* parent = 0);
	QString id() const;

signals:

public slots:
	void readyRead();
	void displayError(QAbstractSocket::SocketError);

private:
	void run();

	ContactList* m_contacts;
	QHash<QString, StatusConnection *> m_connections;

};

#endif // CONTACTSCANNER_H
