// main.cpp
#include "mainwindow.h"
#include <QApplication>
#include "theme.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	app.setStyleSheet(Theme::load());

	MainWindow window;
	window.show();
	return app.exec();
}
