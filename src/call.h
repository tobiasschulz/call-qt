#ifndef CALL_H
#define CALL_H

#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QAudio>
#include <QAudioInput>
#include <QAudioOutput>

#include "contact.h"
#include "connection.h"

class Call: public QObject, public ID
{
Q_OBJECT

public:
	static Call* instance(const Contact& contact);
	QString id() const;
	QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;

	void setConnection(Connection* connection);

signals:
	void started();
	void stopped();

public slots:
	void open();
	void close();
	void onConnected();
	void onDisconnected();
	void onSocketError(QString error, Host host);
	void onConnectFailed(QString error, Host host);

	void notified();
	void handleStateChanged(QAudio::State state);

private:
	explicit Call(const Contact& contact, QObject* parent = 0);
	static QHash<Contact, Call*> m_instances;

	void prepareConnection();
	void open(Connection* host);

	Host m_host;
	Contact m_contact;
	Connection* m_connection;
	QAudioInput* m_audioinput;
	QAudioOutput* m_audiooutput;
};

#endif // CALL_H
