#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QString>

#include "tab.h"
#include "contact.h"

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
	void printLogMessage(QString message);
	void onSendMessage();

private:
	explicit Chat(const Contact& contact, QWidget *parent = 0);
	static QHash<Contact, Chat*> m_instances;

	Ui::Chat *ui;
	Contact m_contact;

	static const QString BEFORE_MESSAGE;
	static const QString AFTER_MESSAGE;

};

#endif // CHAT_H
