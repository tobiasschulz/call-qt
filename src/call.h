#ifndef CALL_H
#define CALL_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QAudio>
#include <QAudioInput>
#include <QAudioOutput>
#include <QPointer>

#include "contact.h"
#include "connection.h"
#include "audioinfo.h"

class Call: public QObject, public ID
{
Q_OBJECT

public:
	static Call* instance(const Contact& contact);
	QString id() const;
	QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;

	void setConnection(Connection* connection);

	enum State
	{
		OPEN, CLOSED, OPENING
	};

signals:
	void started();
	void stopped();
	void statsDurationInput(int);
	void statsDurationOutput(int);
	void statsLatencyInput(int);
	void statsLatencyOutput(int);

public slots:
	void open();
	void close();
	void onConnected();
	void onSocketError(QString error, Host host);
	void onConnectFailed(QString error, Host host);

	void notifiedOutput();
	void notifiedInput();
	void handleStateChanged(QAudio::State state);

private:
	explicit Call(const Contact& contact, QObject* parent = 0);
	static QHash<Contact, Call*> m_instances;

	void prepareConnection();
	void open(Connection* host);

	Host m_host;
	Contact m_contact;
	QPointer<Connection> m_connection;
	QPointer<QAudioInput> m_inputaudio;
	QPointer<QAudioOutput> m_outputaudio;
	QPointer<AudioInfo> m_inputinfo;
	QPointer<AudioInfo> m_outputinfo;
	State m_state;
	QMutex m_openlock;
	QMutex m_closelock;
};

#endif // CALL_H
