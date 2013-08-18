#ifndef CONTACTSCANNER_H
#define CONTACTSCANNER_H

#include <QThread>
#include <QTcpSocket>

class ContactList;
class StatusConnection;

class ContactScanner: public QThread {
Q_OBJECT
public:
	explicit ContactScanner(QObject* parent, ContactList* contacts);

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
