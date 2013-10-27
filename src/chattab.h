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
	static ChatTab* instance(const User& user);
	static ChatTab* instance(const Contact& contact);
	~ChatTab();

	QString tabname() const;
	QIcon tabicon() const;
	QString id() const;
	QString print(PrintFormat format = PRINT_NAME_AND_DATA) const;

	Contact contact() const;
	User user() const;

public slots:
	void onReceivedMessage(QString message);
	void setContact(Contact contact);

private slots:
	void addContact(Contact contact);
	void fillContactCombobox();
	void onComboboxContactChanged(int index);
	void printChatMessage(QString message);
	void onSendMessage();
	void onSendMessageFailed(QString message);
	void onHostStateChanged(Host host);
	void startCall();
	void stopCall();
	void onCallStarted();
	void onCallStopped();

private:
	explicit ChatTab(const User& user);
	static QHash<User, ChatTab*> m_instances;

	bool contactComboboxOutdated();
	void setComboboxContact();
	void updateButtonStates();

	Ui::ChatTab *ui;
	User m_user;
	Contact m_contact;
	QHash<Contact, ChatClient*> m_chatclient;
	Thread m_thread;

	static const QString BEFORE_MESSAGE;
	static const QString AFTER_MESSAGE;

};

#endif // CHATTAB_H
