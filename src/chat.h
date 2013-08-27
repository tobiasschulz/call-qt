#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QString>

#include "tab.h"
#include "contact.h"
#include "chatclient.h"
#include "thread.h"

namespace Ui
{
class Chat;
}

class Chat: public Tab
{
Q_OBJECT

public:
	static Chat* instance(const Contact& contact);
	~Chat();

	QString tabname() const;
	QString id() const;

public slots:
	void printChatMessage(QString message);
	void onSendMessage();
	void onSendMessageFailed(QString message);
	void onReceivedMessage(QString message);

private:
	explicit Chat(const Contact& contact, QWidget *parent = 0);
	static QHash<Contact, Chat*> m_instances;

	Ui::Chat *ui;
	Contact m_contact;
	ChatClient m_client;
	Thread m_thread;

	static const QString BEFORE_MESSAGE;
	static const QString AFTER_MESSAGE;

};

#endif // CHAT_H
