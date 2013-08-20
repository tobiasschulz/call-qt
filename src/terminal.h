#ifndef TERMINAL_H
#define TERMINAL_H

#include <QWidget>
#include <QString>

namespace Ui {
class Terminal;
}

class Terminal: public QWidget {
Q_OBJECT

public:
	explicit Terminal(QWidget *parent = 0);
	~Terminal();

	static const QString TABNAME;

public slots:
	void printLogMessage(QString message);

private:
	Ui::Terminal *ui;
};

#endif // TERMINAL_H
