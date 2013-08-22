#include <QDesktopWidget>
#include <QMessageBox>

#include "maingui.h"
#include "ui_maingui.h"
#include "chat.h"
#include "config.h"

Main::Main(QWidget* parent)
		: QMainWindow(parent), ui(new Ui::Main), m_contactmodel(0), m_tabhash(), m_terminal(0) {

	ui->setupUi(this);
	// window icon, size and position
	this->setWindowIcon(Config::icon("icon"));
	this->resize(900, 400);
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(),
	qApp->desktop()->availableGeometry()));

	// contacts
	m_contactmodel = new ContactModel(this);
	ui->contactlist->setModel(m_contactmodel);
	QObject::connect(ui->contactlist, &QListView::clicked, this, &Main::onContactSelected);
	QObject::connect(this, &Main::shown, m_contactmodel, &ContactModel::onResetContacts);

	// terminal
	m_terminal = new Terminal(this);
	addTab(m_terminal);
	QObject::connect(ui->actionShowTerminal, &QAction::toggled, this, &Main::onShowTerminalToggled);

	// menu
	QObject::connect(ui->actionReloadContacts, &QAction::triggered, m_contactmodel, &ContactModel::resetContacts);
	QObject::connect(ui->actionAbout, &QAction::triggered, this, &Main::onAbout);
	QObject::connect(ui->actionAboutQt, &QAction::triggered, this, &Main::onAboutQt);

	// tab focus
	QObject::connect(ui->tabs, &QTabWidget::currentChanged, this, &Main::onTabChanged);
	onTabChanged(ui->tabs->currentIndex());
}

QString Main::id() const {
	return "Main";
}

void Main::show() {
	QMainWindow::show();
	emit shown();
}

void Main::onShowTerminalToggled(bool checked) {
	if (checked) {
		openTab(m_terminal);
	} else {
		closeTab(m_terminal);
	}
}

void Main::onAbout() {
	QMessageBox::about(this, "About",  "Version " + QCoreApplication::applicationVersion());
}

void Main::onAboutQt() {
	QMessageBox::aboutQt(this, "About Qt");
}

void Main::addTab(Tab* widget) {
	setUpdatesEnabled(false);
	if (widget) {
		int index = ui->tabs->indexOf(widget);
		if (index == -1) {
			m_tabhash[widget->tabname()] = widget;
			ui->tabs->addTab(widget, widget->tabicon(), widget->tabname());
		}
	}
	setUpdatesEnabled(true);
}

void Main::openTab(const QString& tabname) {
	if (m_tabhash.contains(tabname)) {
		openTab(m_tabhash[tabname]);
	}
}

void Main::openTab(Tab* widget) {
	setUpdatesEnabled(false);
	if (!m_tabhash.contains(widget->tabname())) {
		addTab(widget);
	}

	int index = ui->tabs->indexOf(widget);
	if (index == -1) {
		index = ui->tabs->addTab(widget, widget->tabicon(), widget->tabname());
		ui->tabs->setCurrentIndex(index);
	} else if (index == ui->tabs->currentIndex()) {
		// already selected
	} else {
		ui->tabs->setCurrentIndex(index);
	}
	setUpdatesEnabled(true);
}

void Main::closeTab(const QString& tabname) {
	if (m_tabhash.contains(tabname)) {
		log.debug("close tab <tabname=%1>...", tabname);
		closeTab(m_tabhash[tabname]);
	} else {
		log.debug("close tab <tabname=%1>: error: tabname not found", tabname);
	}
}

void Main::closeTab(Tab* widget) {
	setUpdatesEnabled(false);
	int index = ui->tabs->indexOf(widget);
	if (index != -1) {
		log.debug("close tab <widget=%1>...", widget->metaObject()->className());
		ui->tabs->removeTab(index);
	} else {
		log.debug("close tab <widget=%1>: error: widget not found.", widget->metaObject()->className());
	}
	setUpdatesEnabled(true);
}

void Main::onTabChanged(int index) {
	if (index != -1) {
		QWidget* widget = ui->tabs->widget(index);
		if (widget) {
			Tab* tab = (Tab*) widget;
			emit tab->focus();
			this->setWindowTitle(tab->tabname() + " - Build " + QCoreApplication::applicationVersion());
		}
	}
}

void Main::onContactSelected(const QModelIndex & index) {
	Contact contact(m_contactmodel->getContact(index));
	Chat* chattab = Chat::instance(contact);
	log.debug("selected contact: %1 (tab: %2)", contact.id(), chattab->id());
	addTab(chattab);
	openTab(chattab);
}

Main::~Main() {
	delete ui;
}
