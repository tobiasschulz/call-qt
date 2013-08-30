#include <QtGlobal>
#include <QtWidgets>
#include <QSettings>

#include "thread.h"
#include "server.h"
#include "maingui.h"
#include "contact.h"
#include "systemutil.h"
#include "dnscache.h"
#include "config.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	SystemUtil::instance()->messageOutput(type, context, msg);
}

int main(int argv, char** args)
{
	QCoreApplication::setOrganizationName("Tobias Schulz");
	QCoreApplication::setOrganizationDomain("tobias-schulz.eu");
	QCoreApplication::setApplicationName("Call Qt");
	QCoreApplication::setApplicationVersion(Config::instance()->version());
#if !defined(Q_OS_WIN)
	QApplication::setStyle(QStyleFactory::create("Fusion"));
#endif

#if defined(Q_OS_WIN)
	QSettings::setDefaultFormat(QSettings::IniFormat);
#else
	QSettings::setDefaultFormat(QSettings::NativeFormat);
#endif

	qRegisterMetaType<Host>("Host");
	qRegisterMetaTypeStreamOperators<Host>("Host");
	qRegisterMetaType<Contact>("Contact");
	qRegisterMetaTypeStreamOperators<Contact>("Contact");
	qInstallMessageHandler(messageHandler);
	qDebug() << "starting app";

	QApplication app(argv, args);
	DnsCache::instance();
	Thread serverthread("ServerMain");
	serverthread.start();
	Server server;
	server.moveToThread(&serverthread);
	Main* main = Main::instance();
	QObject::connect(main, SIGNAL(shown()), &server, SLOT(start()));
	main->show();
	return app.exec();
}
