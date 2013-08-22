#include <QtWidgets>
#include <QtGlobal>

#include "server.h"
#include "maingui.h"
#include "contact.h"
#include "systemutil.h"
#include "config.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	SystemUtil::instance()->messageOutput(type, context, msg);
}

int main(int argv, char** args) {
	QCoreApplication::setOrganizationName("Tobias Schulz");
	QCoreApplication::setOrganizationDomain("tobias-schulz.eu");
	QCoreApplication::setApplicationName("Call Qt");
	QCoreApplication::setApplicationVersion(Config::version());
	qRegisterMetaType<Host>("Host");
	qRegisterMetaTypeStreamOperators<Host>("Host");
	qRegisterMetaType<Contact>("Contact");
	qRegisterMetaTypeStreamOperators<Contact>("Contact");
	qInstallMessageHandler(messageHandler);
	qDebug() << "starting app";

	QApplication app(argv, args);
	app.setApplicationName("Audio Device Test");
	Server server;
	Main main;
	QObject::connect(&main, &Main::shown, &server, &Server::start);
	main.show();
	return app.exec();
}
