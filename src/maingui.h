#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>

#include "contactmodel.h"

namespace Ui {
class Main;
}

class Main: public QMainWindow {
Q_OBJECT

public:
	explicit Main(QWidget* parent = 0);
	~Main();

private:
	Ui::Main* ui;

	ContactModel* contactmodel;
};

#endif // MAIN_H
