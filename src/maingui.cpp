#include <QDesktopWidget>

#include "maingui.h"
#include "ui_maingui.h"

Main::Main(QWidget* parent)
		: QMainWindow(parent), ui(new Ui::Main) {
	ui->setupUi(this);
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(),
	qApp->desktop()->availableGeometry()));
	contactmodel = new ContactModel(this);
	//ui->contactlist->setModel(contactmodel);
	///QStandardItemModel model();
	//ui->listView->setModel(model);
}

Main::~Main() {
	delete ui;
}
