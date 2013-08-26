#include <QMutex>
#include <QTimer>

#include "chat.h"
#include "ui_chat.h"
#include "config.h"

const QString Chat::BEFORE_MESSAGE = "<div style='font-family: monospace; font-size: 9pt;'>";
const QString Chat::AFTER_MESSAGE = "</div>";

QHash<Contact, Chat*> Chat::m_instances;

Chat* Chat::instance(const Contact& contact)
{
	static QMutex mutex;
	mutex.lock();
	if (!m_instances.contains(contact)) {
		m_instances[contact] = new Chat(contact);
	}
	mutex.unlock();
	return m_instances[contact];
}

Chat::Chat(const Contact& contact, QWidget *parent)
		: Tab("Chat", Config::icon("user-available"), parent), ui(new Ui::Chat), m_contact(contact)
{
	ui->setupUi(this);

	// QObject::connect(this, &Chat::focus, ui->chatinput, static_cast<void (QLineEdit::*)()>(&QLineEdit::setFocus));
	QObject::connect(this, SIGNAL(focus()), ui->chatinput, SLOT(setFocus()));
	QObject::connect(ui->chatinput, &QLineEdit::returnPressed, this, &Chat::onSendMessage);
	QTimer::singleShot(0, ui->chatinput, SLOT(setFocus()));
}

Chat::~Chat()
{
	delete ui;
}

void Chat::onSendMessage()
{

}

void Chat::printLogMessage(QString message)
{
	ui->chatlog->append(BEFORE_MESSAGE + message.trimmed().toHtmlEscaped() + AFTER_MESSAGE);
}

QString Chat::tabname() const
{
	return m_contact.toString();
}

QString Chat::id() const
{
	return "Chat<" + m_contact.id() + ">";
}
