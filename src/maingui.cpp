#include <QDesktopWidget>
#include <QMessageBox>

#include "maingui.h"
#include "ui_maingui.h"
#include "terminal.h"
#include "config.h"

Main::Main(QWidget* parent)
		: QMainWindow(parent), ui(new Ui::Main), contactmodel(0), tabhash() {
	ui->setupUi(this);
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(),
	qApp->desktop()->availableGeometry()));

	contactmodel = new ContactModel(this);
	ui->contactlist->setModel(contactmodel);

	Terminal* terminal = new Terminal(this);
	addTab(terminal, Terminal::TABNAME);
	QObject::connect(ui->actionShowTerminal, &QAction::toggled, this, &Main::onShowTerminalToggled);
	QObject::connect(SystemUtil::instance(), &SystemUtil::newLogMessage, terminal, &Terminal::printLogMessage);

	QObject::connect(ui->actionReloadContacts, &QAction::triggered, contactmodel, &ContactModel::resetContacts);
	QObject::connect(ui->actionAbout, &QAction::triggered, this, &Main::onAbout);
	QObject::connect(ui->actionAboutQt, &QAction::triggered, this, &Main::onAboutQt);

	///QStandardItemModel model();
	//ui->listView->setModel(model);
}

QString Main::id() const {
	return "Main";
}

void Main::onShowTerminalToggled(bool checked) {
	if (checked) {
		openTab(Terminal::TABNAME);
	} else {
		closeTab(Terminal::TABNAME);
	}
}

void Main::onAbout() {
	QMessageBox::about(this, "About", "");
}

void Main::onAboutQt() {
	QMessageBox::aboutQt(this, "About Qt");
}

void Main::addTab(QWidget* widget, const QString& tabname) {
	tabhash[tabname] = widget;
	ui->tabs->addTab(widget, tabname);
}

void Main::openTab(const QString& tabname) {
	if (tabhash.contains(tabname)) {
		QWidget* widget = tabhash[tabname];
		int index = ui->tabs->indexOf(widget);
		if (index != -1 && index != ui->tabs->currentIndex()) {
			ui->tabs->setCurrentIndex(index);
		} else {
			index = ui->tabs->addTab(widget, tabname);
			ui->tabs->setCurrentIndex(index);
		}
	}
}

void Main::openTab(QWidget* widget) {
	foreach (const QString& tabname, tabhash.keys())
	{
		if (widget == tabhash[tabname]) {
			openTab(tabname);
			break;
		}
	}
}

void Main::closeTab(const QString& tabname) {
	if (tabhash.contains(tabname)) {
		log.debug("close tab <tabname=%1>...", tabname);
		closeTab(tabhash[tabname]);
	} else {
		log.debug("close tab <tabname=%1>: error: tabname not found", tabname);
	}
}

void Main::closeTab(QWidget* widget) {
	int index = ui->tabs->indexOf(widget);
	if (index != -1) {
		log.debug("close tab <widget=%1>...", widget->metaObject()->className());
		ui->tabs->removeTab(index);
	} else {
		log.debug("close tab <widget=%1>: error: widget not found.", widget->metaObject()->className());
	}
}

Main::~Main() {
	delete ui;
}
