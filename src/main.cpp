#include <QtWidgets>

#include "maingui.h"

int main(int argv, char** args) {
	QApplication app(argv, args);
	app.setApplicationName("Audio Device Test");
	Main main;
	main.show();
	return app.exec();
}
