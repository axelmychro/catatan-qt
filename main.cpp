// main.cpp
#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QFile styleFile(":/main.qss");
	if (styleFile.open(QIODevice::ReadOnly)) {
		app.setStyleSheet(styleFile.readAll());
	}

	MainWindow window;
	window.show();
	return app.exec();
}
