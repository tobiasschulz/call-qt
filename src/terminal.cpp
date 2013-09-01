#include <QSettings>
#include <QColor>

#include "terminal.h"
#include "ui_terminal.h"
#include "systemutil.h"
#include "config.h"

const QString Terminal::BEFORE_MESSAGE = "<div style='font-family: monospace; font-size: 9pt;'>";
const QString Terminal::AFTER_MESSAGE = "</div>";

Terminal::Terminal()
		: Tab("Terminal", Config::instance()->icon("console")), ui(new Ui::Terminal)
{

	ui->setupUi(this);
	QObject::connect(SystemUtil::instance(), &SystemUtil::newLogMessage, this, &Terminal::printLogMessage);
	QObject::connect(this, SIGNAL(focus()), this, SLOT(setFocus()));
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
		return Qt::darkRed;
	else if (type == "(info)")
		return Qt::darkGreen;
	else if (type == "(warn)")
		return Qt::darkRed;
	else
		return Qt::darkBlue;
}

QString color(const QString& text, const QColor& color)
{
	return "<span style='color: " + color.name() + ";'>" + text.toHtmlEscaped() + "</span>";
}

void Terminal::printLogMessage(QString message)
{
	message = message.trimmed();
	QStringList parts = message.split(' ', QString::SkipEmptyParts);
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
		message = color(datetime, Qt::darkGray) + " " + color(type, typeColor(parts[2])) + " "
				+ color(threadname, Qt::darkYellow) + color(classname, QColor("#E56717")) + color(text, Qt::black);
	} else {
		message = message.toHtmlEscaped();
	}
	if (ui->terminal->document()->lineCount() > 1000)
		ui->terminal->clear();
	ui->terminal->append(BEFORE_MESSAGE + message + AFTER_MESSAGE);
}

QString Terminal::id() const
{
	return "Terminal";
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

