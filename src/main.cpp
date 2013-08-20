#include <QtWidgets>
#include <QtGlobal>

#include "server.h"
#include "maingui.h"
#include "systemutil.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	SystemUtil::instance()->messageOutput(type, context, msg);
}

int main(int argv, char** args) {
	qInstallMessageHandler(messageHandler);
	qDebug() << "starting app";

	QApplication app(argv, args);
	app.setApplicationName("Audio Device Test");
	Server server;
	Main main;
	main.show();
	return app.exec();
}
