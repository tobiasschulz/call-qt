#ifndef CHATTAB_H
#define CHATTAB_H

#include <QWidget>
#include <QString>

#include "tab.h"
#include "contact.h"
#include "chatclient.h"
#include "thread.h"

namespace Ui
{
class ChatTab;
}

class ChatTab: public Tab
{
Q_OBJECT

public:
	static ChatTab* instance(const Contact& contact);
	~ChatTab();

	QString tabname() const;
	QIcon tabicon() const;
	QString id() const;
	QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;

	Contact contact() const;

public slots:
	void printChatMessage(QString message);
	void onSendMessage();
	void onSendMessageFailed(QString message);
	void onReceivedMessage(QString message);
	void onHostOnline(Host host);
	void onHostOffline(Host host);
	void startCall();
	void stopCall();
	void onCallStarted();
	void onCallStopped();

private:
	explicit ChatTab(const Contact& contact);
	static QHash<Contact, ChatTab*> m_instances;

	Ui::ChatTab *ui;
	Contact m_contact;
	ChatClient m_chatclient;
	Thread m_thread;

	static const QString BEFORE_MESSAGE;
	static const QString AFTER_MESSAGE;

};

#endif // CHATTAB_H
