#include "terminal.h"
#include "ui_terminal.h"
#include "systemutil.h"
#include "config.h"

const QString Terminal::BEFORE_MESSAGE = "<div style='font-family: monospace; font-size: 9pt;'>";
const QString Terminal::AFTER_MESSAGE = "</div>";

Terminal::Terminal(QWidget *parent)
		: Tab("Terminal", Config::icon("console"), parent), ui(new Ui::Terminal)
{

	ui->setupUi(this);
	QObject::connect(SystemUtil::instance(), &SystemUtil::newLogMessage, this, &Terminal::printLogMessage);
	QObject::connect(this, SIGNAL(focus()), this, SLOT(setFocus()));
}

Terminal::~Terminal()
{
	delete ui;
}

void Terminal::printLogMessage(QString message)
{
	ui->terminal->append(BEFORE_MESSAGE + message.trimmed().toHtmlEscaped() + AFTER_MESSAGE);
}

QString Terminal::id() const
{
	return "Terminal";
}
