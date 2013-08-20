#include "terminal.h"
#include "ui_terminal.h"

const QString Terminal::TABNAME = "Terminal";

Terminal::Terminal(QWidget *parent)
		: QWidget(parent), ui(new Ui::Terminal) {
	ui->setupUi(this);
}

Terminal::~Terminal() {
	delete ui;
}

void Terminal::printLogMessage(QString message) {
	ui->terminal->append(message.trimmed());
}
