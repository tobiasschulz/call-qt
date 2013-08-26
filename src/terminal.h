#ifndef TERMINAL_H
#define TERMINAL_H

#include <QWidget>
#include <QString>

#include "tab.h"

namespace Ui
{
class Terminal;
}

class Terminal: public Tab
{
Q_OBJECT

public:
	explicit Terminal(QWidget *parent = 0);
	~Terminal();

	QString id() const;

public slots:
	void printLogMessage(QString message);

private:
	Ui::Terminal *ui;

	static const QString BEFORE_MESSAGE;
	static const QString AFTER_MESSAGE;
};

#endif // TERMINAL_H
