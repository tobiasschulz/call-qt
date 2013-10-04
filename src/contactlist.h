#ifndef CONTACTLIST_Httateate
#define CONTACTLIST_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QSet>
#include <QMutex>
#include <QMutexLocker>

#include "contact.h"
#include "connection.h"

class ContactList: public QObject, public ID
{
Q_OBJECT
public:
	static ContactList* instance();
	QString id() const;

	const Contact& contact(int index) const;
	const Contact& reachableContact(const Contact& contact) const;
	int size() const;
	void addSignals(Connection* connection);
	QStringList unknownHosts();

	enum HostState
	{
		HOST_OFFLINE, CONNECTING, HOST_ONLINE
	};
	typedef QSet<HostState> HostStateSet;

	bool isHostOnline(Host host);
	HostStateSet hostState(Host host);
	HostStateSet hostState(QString hostname);

signals:
	void hostOnline(Host host);
	void hostOffline(Host host);

	void hostChanged(Host host);
	void contactStateChanged(int i);
	void unknownHostStateChanged(int i);

	void beginInsertItems(int start, int end);
	void endInsertItems();
	void beginRemoveItems(int start, int end);
	void endRemoveItems();

public slots:
	void addContact(Contact contact);
	void onResetContacts();
	void setHostOnline(Host host);
	void setHostOffline(Host host);
	void addHostState(Host host, HostState state);
	void removeHostState(Host host, HostState state);

private slots:
	void onHostStateChanged(Host host);

private:
	explicit ContactList(QObject* parent = 0);

	void buildSortedList();

	static ContactList* m_instance;
	QList<Contact> m_contacts;
	QHash<Host, HostStateSet> m_hoststate;
	QStringList m_unknownhosts;
	static QMutex m_lock;
};

#endif // CONTACTLIST_H
