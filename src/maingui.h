#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>

#include "contactmodel.h"
#include "systemutil.h"
#include "log.h"

namespace Ui {
class Main;
}

class Main: public QMainWindow, public Id {
Q_OBJECT

public:
	explicit Main(QWidget* parent = 0);
	~Main();
	QString id() const;

public slots:
	void addTab(QWidget* widget, const QString& tabname);
	void openTab(const QString& tabname);
	void openTab(QWidget* widget);
	void closeTab(const QString& tabname);
	void closeTab(QWidget* widget);

	void onShowTerminalToggled(bool checked);
	void onAbout();
	void onAboutQt();

private:
	Ui::Main* ui;
	ContactModel* contactmodel;
	QHash<QString, QWidget*> tabhash;
};

#endif // MAIN_H
