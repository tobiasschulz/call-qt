#include "terminal.h"
#include "ui_terminal.h"
#include "config.h"

const QString Terminal::BEFORE_MESSAGE = "<div style='font-family: monospace; font-size: 9pt;'>";
const QString Terminal::AFTER_MESSAGE = "</div>";

Terminal::Terminal(QWidget *parent)
		: Tab("Terminal", Config::icon("console"), parent), ui(new Ui::Terminal) {
	ui->setupUi(this);
}

Terminal::~Terminal() {
	delete ui;
}

void Terminal::printLogMessage(QString message) {
	ui->terminal->append(BEFORE_MESSAGE + message.trimmed().toHtmlEscaped() + AFTER_MESSAGE);
}

QString Terminal::id() const {
	return "Terminal";
}
