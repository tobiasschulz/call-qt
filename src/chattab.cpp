#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include <QSettings>

#include "ui_chattab.h"
#include "chattab.h"
#include "contactlist.h"
#include "call.h"
#include "config.h"
#include "maingui.h"

const QString ChatTab::BEFORE_MESSAGE = "<div style='font-family: monospace; font-size: 9pt;'>";
const QString ChatTab::AFTER_MESSAGE = "</div>";

QHash<User, ChatTab*> ChatTab::m_instances;

QIcon iconHostOnlineOffline(const Contact& contact)
{
	return HostStates()->isHostOnline(contact.host()) ?
			Config::instance()->icon("user-available") : Config::instance()->icon("user-offline");
}

ChatTab* ChatTab::instance(const User& user)
{
	static QMutex mutex;
	QMutexLocker locker(&mutex);
	ChatTab* instance = 0;
	if (m_instances.contains(user)) {
		instance = m_instances[user];
	} else {
		instance = new ChatTab(user);
		m_instances[user] = instance;
	}
	return instance;
}

ChatTab* ChatTab::instance(const Contact& contact)
{
	ChatTab* instance = ChatTab::instance(contact.user());
	instance->addContact(contact);
	return instance;
}

ChatTab::ChatTab(const User& user)
		: Tab("Chat", QIcon()), ui(new Ui::ChatTab), m_user(user), m_contact(Contact::INVALID_CONTACT), m_chatclient(),
			m_thread("ChatThread")
{
	ui->setupUi(this);
	m_thread.start();

	// contactlist-related signals
	QObject::connect(ui->contacts, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboboxContactChanged(int)));

	// chat-related signals
	QObject::connect(this, SIGNAL(focus()), ui->chatinput, SLOT(setFocus()));
	QObject::connect(ui->chatbutton, &QPushButton::clicked, this, &ChatTab::onSendMessage);
	QObject::connect(ui->chatinput, &QLineEdit::returnPressed, this, &ChatTab::onSendMessage);
	QTimer::singleShot(0, ui->chatinput, SLOT(setFocus()));

	// contact-related signals
	QObject::connect(HostStates(), &List::Hosts::hostOnline, this, &ChatTab::onHostStateChanged);
	QObject::connect(HostStates(), &List::Hosts::hostOffline, this, &ChatTab::onHostStateChanged);

	// call-related signals
	QObject::connect(ui->callbutton_start, &QPushButton::clicked, this, &ChatTab::startCall);
	QObject::connect(ui->callbutton_stop, &QPushButton::clicked, this, &ChatTab::stopCall);
	ui->callbutton_stop->hide();

	// stats-related signals
	QSettings settings;
	bool showStats = settings.value("window/show-stats", true).toBool();
	QObject::connect(ui->actionShowStats, &QCheckBox::toggled, Main::instance(), &Main::onShowStatsToggled);
	ui->actionShowStats->setChecked(showStats);
}

void ChatTab::addContact(Contact contact)
{
	static QSet<Contact> registeredContacts;
	if (!registeredContacts.contains(contact)) {
		log.debug("addContact: %1", Log::print(contact));
		registeredContacts << contact;

		m_chatclient[contact] = new ChatClient(contact);
		m_chatclient[contact]->moveToThread(&m_thread);

		QObject::connect(m_chatclient[contact], &ChatClient::sendMessageFailed, this, &ChatTab::onSendMessageFailed);
		QObject::connect(m_chatclient[contact], &ChatClient::receivedMessage, this, &ChatTab::onReceivedMessage);
		QTimer::singleShot(0, m_chatclient[contact], SLOT(connect()));

		QObject::connect(Call::instance(contact), &Call::started, this, &ChatTab::onCallStarted);
		QObject::connect(Call::instance(contact), &Call::stopped, this, &ChatTab::onCallStopped);

		fillContactCombobox();
	}
}

void ChatTab::setContact(Contact contact)
{
	addContact(contact);

	log.debug("setContact: %1", Log::print(contact));
	m_contact = contact;
	setComboboxContact();
	emit tabIconChanged();
}

void ChatTab::setComboboxContact()
{
	QComboBox* list = ui->contacts;
	list->setUpdatesEnabled(false);
	int currentIndex = list->currentIndex();
	int newIndex = list->findText(m_contact.host().toString());
	if (newIndex == -1) {
		log.warn("setComboboxContact failed: contact=%1, text=%2, newIndex=%3, currentIndex=%4", Log::print(m_contact),
				m_contact.host().toString(), QString::number(newIndex), QString::number(currentIndex));
	} else if (newIndex == currentIndex) {
		// useless
	} else {
		log.debug("setComboboxContact: contact=%1, text=%2", Log::print(m_contact), m_contact.host().toString());
		list->setCurrentIndex(newIndex);
	}
	list->setUpdatesEnabled(true);
}

bool ChatTab::contactComboboxOutdated()
{
	QComboBox* list = ui->contacts;
	if (list->count() == m_user.hosts().size()) {
		// right size, at least
		int i = 0;
		foreach (const Contact& contact, m_user.contacts())
		{
			QIcon icon = iconHostOnlineOffline(contact);
			if (list->itemText(i) != contact.host().toString() || list->itemIcon(i).name() != icon.name()) {
				// different item
				return true;
			}
			++i;
		}
		return false;
	} else {
		// wrong size
		return true;
	}
}

void ChatTab::fillContactCombobox()
{
	QComboBox* list = ui->contacts;
	if (contactComboboxOutdated()) {
		list->setUpdatesEnabled(false);
		list->clear();
		foreach (const Contact& contact, m_user.contacts())
		{
			if (contact.host().isReachable()) {
				list->addItem(iconHostOnlineOffline(contact), contact.host().toString(), QVariant::fromValue(contact));
			}
		}
		if (list->count() == 0) {
			foreach (const Contact& contact, m_user.contacts())
			{
				if (contact.host().isUnreachable()) {
					list->addItem(iconHostOnlineOffline(contact), contact.host().toString(),
							QVariant::fromValue(contact));
				}
			}
		}
		list->setUpdatesEnabled(true);
	}
}

void ChatTab::onComboboxContactChanged(int index)
{
	if (index != -1) {
		QVariant variant = ui->contacts->itemData(index, Qt::UserRole);
		if (variant.isValid() && variant.canConvert<Contact>()) {
			Contact contact = variant.value<Contact>();
			log.debug("onContactChanged: index=%1, contact=%2", QString::number(index), Log::print(contact));
			setContact(contact);
			updateButtonStates();
		} else {
			log.debug("onContactChanged: index=%1, contact=%2", QString::number(index), "invalid QVariant");
		}
	} else {
		log.debug("onContactChanged: index=%1, contact=%2", QString::number(index), "out of range");
	}
}

ChatTab::~ChatTab()
{
	delete ui;
}

void ChatTab::startCall()
{
	if (m_contact != Contact::INVALID_CONTACT) {
		log.debug("startCall(%1)", Log::print(m_contact));
		QTimer::singleShot(0, Call::instance(m_contact), SLOT(open()));
	} else {
		log.debug("startCall(%1)", "invalid contact");
	}
}

void ChatTab::stopCall()
{
	if (m_contact != Contact::INVALID_CONTACT) {
		foreach (const Contact& contact, m_user.contacts())
		{
			log.debug("stopCall(%1)", Log::print(contact));
			QTimer::singleShot(0, Call::instance(contact), SLOT(close()));
		}
	} else {
		log.debug("stopCall(%1)", "invalid contact");
	}
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
	emit m_chatclient[m_contact]->sendMessage(message);
	printChatMessage("send (" + m_contact.address().toString() + "): " + message);
}
void ChatTab::onSendMessageFailed(QString message)
{
	ui->chatinput->setEnabled(false);
	ui->chatinput->setText(message);
	ui->chatinput->setCursorPosition(message.size());
	ui->chatinput->setEnabled(true);
	ui->chatinput->setFocus();
	printChatMessage("send failed (" + m_contact.address().toString() + "): " + message);
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
	return m_user.fullname() + "@" + m_user.computername();
}
QIcon ChatTab::tabicon() const
{
	return iconHostOnlineOffline(m_contact);
}
QString ChatTab::id() const
{
	return "ChatTab<" + m_user.id() + ">";
}
QString ChatTab::print(PrintFormat format) const
{
	QString data = m_user.print(ID::PRINT_ONLY_DATA);

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

void ChatTab::onHostStateChanged(Host host)
{
	if (m_user.hosts().contains(host)) {
		emit tabIconChanged();
		fillContactCombobox();
		updateButtonStates();
	}
}

void ChatTab::updateButtonStates()
{
	bool online = HostStates()->isHostOnline(m_contact.host());
	ui->callbutton_start->setEnabled(online);
	ui->chatinput->setEnabled(online);
	ui->chatbutton->setEnabled(online);
}

