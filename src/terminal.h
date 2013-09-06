#ifndef TERMINAL_H
#define TERMINAL_H

#include <QWidget>
#include <QString>

#include "tab.h"

namespace Ui
{
class Terminal;
}

class Message
{
public:
	explicit Message(ID::Verbosity type, QString thread, QString message);

	ID::Verbosity m_type;
	QString m_thread;
	QString m_message;
};

class Terminal: public Tab
{
Q_OBJECT

public:
	explicit Terminal();
	~Terminal();

	QString id() const;

public slots:
	void newLogMessage(ID::Verbosity type, QString thread, QString message);
	void checkboxStateChanged(int state);
	void reprintLogMessages();
	virtual void opened();
	virtual void closed();

private:
	Ui::Terminal *ui;
	QList<Message> messages;
	bool logDebug;
	bool logInfo;
	bool logWarning;
	bool logError;

	void printLogMessage(const Message& message);

	static const QString BEFORE_MESSAGE;
	static const QString AFTER_MESSAGE;
};

#endif // TERMINAL_H
