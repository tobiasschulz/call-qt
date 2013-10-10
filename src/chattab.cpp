#include <QMutex>
#include <QMutexLocker>
#include <QTimer>

#include "ui_chattab.h"
#include "chattab.h"
#include "contactlist.h"
#include "call.h"
#include "config.h"

const QString ChatTab::BEFORE_MESSAGE = "<div style='font-family: monospace; font-size: 9pt;'>";
const QString ChatTab::AFTER_MESSAGE = "</div>";

QHash<Contact, ChatTab*> ChatTab::m_instances;

ChatTab* ChatTab::instance(const Contact& contact)
{
	static QMutex mutex;
	QMutexLocker locker(&mutex);
	ChatTab* instance = 0;
	if (m_instances.contains(contact)) {
		instance = m_instances[contact];
	} else {
		instance = new ChatTab(contact);
		m_instances[contact] = instance;
	}
	return instance;
}

ChatTab::ChatTab(const Contact& contact)
		: Tab("Chat", QIcon()), ui(new Ui::ChatTab), m_contact(contact), m_chatclient(contact), m_thread("ChatThread")
{
	ui->setupUi(this);
	m_thread.start();
	m_chatclient.moveToThread(&m_thread);
	// moveToThread(&m_thread);

	// QObject::connect(this, &Chat::focus, ui->chatinput, static_cast<void (QLineEdit::*)()>(&QLineEdit::setFocus));

	// chat-related signals
	QObject::connect(this, SIGNAL(focus()), ui->chatinput, SLOT(setFocus()));
	QObject::connect(ui->chatbutton, &QPushButton::clicked, this, &ChatTab::onSendMessage);
	QObject::connect(ui->chatinput, &QLineEdit::returnPressed, this, &ChatTab::onSendMessage);
	QObject::connect(&m_chatclient, &ChatClient::sendMessageFailed, this, &ChatTab::onSendMessageFailed);
	QObject::connect(&m_chatclient, &ChatClient::receivedMessage, this, &ChatTab::onReceivedMessage);
	QTimer::singleShot(0, ui->chatinput, SLOT(setFocus()));
	QTimer::singleShot(0, &m_chatclient, SLOT(connect()));

	// contact-related signals
	QObject::connect(ContactList::hosts(), &List::Hosts::hostOnline, this, &ChatTab::onHostOnline);
	QObject::connect(ContactList::hosts(), &List::Hosts::hostOffline, this, &ChatTab::onHostOffline);

	// call-related signals
	QObject::connect(ui->callbutton_start, &QPushButton::clicked, this, &ChatTab::startCall);
	QObject::connect(ui->callbutton_stop, &QPushButton::clicked, this, &ChatTab::stopCall);
	ui->callbutton_stop->hide();
	QObject::connect(Call::instance(m_contact), &Call::started, this, &ChatTab::onCallStarted);
	QObject::connect(Call::instance(m_contact), &Call::stopped, this, &ChatTab::onCallStopped);
}

ChatTab::~ChatTab()
{
	delete ui;
}

void ChatTab::startCall()
{
	log.debug("startCall()");
	QTimer::singleShot(0, Call::instance(m_contact), SLOT(open()));
}

void ChatTab::stopCall()
{
	log.debug("stopCall()");
	QTimer::singleShot(0, Call::instance(m_contact), SLOT(close()));
}

void ChatTab::onCallStarted()
{
	ui->callbutton_start->hide();
	ui->callbutton_stop->show();
}

void ChatTab::onCallStopped()
{
	ui->callbutton_stop->hide();
	ui->callbutton_start->show();
}

void ChatTab::onSendMessage()
{
	ui->chatinput->setEnabled(false);
	QString message = ui->chatinput->text();
	ui->chatinput->setText("");
	ui->chatinput->setEnabled(true);
	ui->chatinput->setFocus();
	emit m_chatclient.sendMessage(message);
	printChatMessage("send: " + message);
}
void ChatTab::onSendMessageFailed(QString message)
{
	ui->chatinput->setEnabled(false);
	ui->chatinput->setText(message);
	ui->chatinput->setCursorPosition(message.size());
	ui->chatinput->setEnabled(true);
	ui->chatinput->setFocus();
	printChatMessage("send failed: " + message);
}
void ChatTab::onReceivedMessage(QString message)
{
	printChatMessage("received: " + message);
}

void ChatTab::printChatMessage(QString message)
{
	ui->chatlog->append(BEFORE_MESSAGE + message.trimmed().toHtmlEscaped() + AFTER_MESSAGE);
}

QString ChatTab::tabname() const
{
	return m_contact.toString();
}
QIcon ChatTab::tabicon() const
{
	return ContactList::hosts()->isHostOnline(m_contact.host()) ?
			Config::instance()->icon("user-available") : Config::instance()->icon("user-offline");
}
QString ChatTab::id() const
{
	return "ChatTab<" + m_contact.id() + ">";
}
QString ChatTab::print(PrintFormat format) const
{
	QString data = m_contact.print(ID::PRINT_ONLY_DATA);

	if (format == PRINT_ONLY_NAME)
		return "ChatTab";
	else if (format == PRINT_ONLY_DATA)
		return data;
	else
		return "ChatTab " + data;
}

Contact ChatTab::contact() const
{
	return m_contact;
}

void ChatTab::onHostOnline(Host host)
{
	if (m_contact.host() == host) {
		emit tabIconChanged();
	}
}
void ChatTab::onHostOffline(Host host)
{
	if (m_contact.host() == host) {
		emit tabIconChanged();
	}
}
