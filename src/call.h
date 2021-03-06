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

#include "id.h"
#include "contact.h"
#include "connection.h"
#include "audioinfo.h"
#include "thread.h"

class Call: public QObject, public ID
{
Q_OBJECT

public:
	static Call* instance(const Contact& contact);
	virtual ~Call();
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
	void statsContact(Contact contact);
	void statsDurationInput(int);
	void statsDurationOutput(int);
	void statsLatencyInput(int);
	void statsLatencyOutput(int);
	void statsFormatInput(QAudioFormat format);
	void statsFormatOutput(QAudioFormat format);

public slots:
	void open();
	void close();
	void onConnected();
	void startSpeaker();
	void onSocketError(QString error, Host host);
	void onConnectFailed(QString error, Host host);

	void notifiedOutput();
	void notifiedInput();
	void handleInputStateChanged(QAudio::State state);
	void handleOutputStateChanged(QAudio::State state);
	void restartSpeaker();

	void onVolumeChangedInput(qreal volume);
	void onVolumeChangedOutput(qreal volume);

private:
	explicit Call(const Contact& contact, QObject* parent = 0);
	static QHash<Contact, Call*> m_instances;
	static QHash<Contact, Thread*> m_threads;

	void prepareConnection();
	void open(Connection* host);
	void printStates();

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
