#include "maingui.h"
#include "ui_maingui.h"

Main::Main(QWidget* parent)
		: QMainWindow(parent), ui(new Ui::Main) {
	ui->setupUi(this);
	contactmodel = new ContactModel(this);
	//ui->contactlist->setModel(contactmodel);
	///QStandardItemModel model();
	//ui->listView->setModel(model);
}

Main::~Main() {
	delete ui;
}
