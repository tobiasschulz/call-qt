#include <QSettings>
#include <QTimer>
#include <QColor>

#include "terminal.h"
#include "ui_terminal.h"
#include "systemutil.h"
#include "config.h"

const QString Terminal::BEFORE_MESSAGE = "<div style='font-family: monospace; font-size: 9pt;'>";
const QString Terminal::AFTER_MESSAGE = "</div>";

Terminal::Terminal()
		: Tab("Terminal", Config::instance()->icon("console")), ui(new Ui::Terminal), messages(), logDebug(true),
			logInfo(true), logWarning(true), logError(true)
{

	ui->setupUi(this);
	QObject::connect(SystemUtil::instance(), &SystemUtil::newLogMessage, this, &Terminal::newLogMessage);
	QObject::connect(this, SIGNAL(focus()), this, SLOT(setFocus()));

	QObject::connect(ui->logDebug, &QCheckBox::stateChanged, this, &Terminal::checkboxStateChanged);
	QObject::connect(ui->logInfo, &QCheckBox::stateChanged, this, &Terminal::checkboxStateChanged);
	QObject::connect(ui->logWarning, &QCheckBox::stateChanged, this, &Terminal::checkboxStateChanged);
	QObject::connect(ui->logError, &QCheckBox::stateChanged, this, &Terminal::checkboxStateChanged);
}

Terminal::~Terminal()
{
	delete ui;
}

QColor typeColor(const QString& type)
{
	if (type == "(debug)")
		return Qt::darkCyan;
	else if (type == "(error)")
		return Qt::red;
	else if (type == "(info)")
		return Qt::darkGreen;
	else if (type == "(warn)")
		return Qt::red;
	else
		return Qt::darkBlue;
}

QString color(const QString& text, const QColor& color)
{
	return "<span style='color: " + color.name() + ";'>" + text.toHtmlEscaped() + "</span>";
}

void Terminal::newLogMessage(ID::Verbosity type, QString thread, QString message)
{
	if (messages.size() > 5000) {
		messages = messages.mid(2500);
		reprintLogMessages();
	}
	Message msg(type, thread, message);
	messages.append(msg);
	printLogMessage(msg);
}

void Terminal::reprintLogMessages()
{
	ui->terminal->clear();
	foreach (const Message& msg, messages)
	{
		printLogMessage(msg);
	}
}

void Terminal::printLogMessage(const Message& msg)
{
	if ((msg.m_type == ID::DEBUG && logDebug) || (msg.m_type == ID::INFO && logInfo)
			|| (msg.m_type == ID::WARNING && logWarning) || (msg.m_type == ID::ERROR && logError)) {
		ui->terminal->append(BEFORE_MESSAGE + msg.m_message + AFTER_MESSAGE);
	}
}

QString Terminal::id() const
{
	return "Terminal";
}

void Terminal::checkboxStateChanged(int state)
{
	Q_UNUSED(state);
	logDebug = ui->logDebug->checkState() == Qt::Checked;
	logInfo = ui->logInfo->checkState() == Qt::Checked;
	logWarning = ui->logWarning->checkState() == Qt::Checked;
	logError = ui->logError->checkState() == Qt::Checked;

	QTimer::singleShot(0, this, SLOT(reprintLogMessages()));
}

void Terminal::opened()
{
	log.debug("Terminal::opened()");
	QSettings settings;
	settings.setValue("window/show-terminal", true);
}

void Terminal::closed()
{
	QSettings settings;
	settings.setValue("window/show-terminal", false);
}

Message::Message(ID::Verbosity type, QString thread, QString message)
		: m_type(type), m_thread(thread), m_message()
{
	QStringList parts = message.trimmed().split(' ', QString::SkipEmptyParts);
	if (parts.size() >= 5) {
		const QString& datetime = parts[0] + " " + parts[1];
		const QString& type = parts[2];
		const QString& threadname = parts[3];
		int i = 4;
		QString classname = "";
		for (; i < parts.size() && !parts[i - 1].endsWith("]:"); ++i) {
			classname += " " + parts[i];
		}
		QString text = "";
		for (; i < parts.size(); ++i) {
			text += " " + parts[i];
		}
		m_message = color(datetime, Qt::darkGray) + " " + color(type, typeColor(parts[2])) + " "
				+ color(threadname, Qt::darkYellow) + color(classname, QColor("#E56717")) + color(text, Qt::black);
	} else {
		m_message = message.toHtmlEscaped();
	}
}

