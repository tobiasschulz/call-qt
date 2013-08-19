#include <QtWidgets>
#include <QtGlobal>

#include "server.h"
#include "maingui.h"
#include "util.h"

int main(int argv, char** args) {
	qInstallMessageHandler(SystemUtil::messageOutput);
	qDebug() << "starting app";

	QApplication app(argv, args);
	app.setApplicationName("Audio Device Test");
	Server server;
	Main main;
	main.show();
	return app.exec();
}
