#include "mainwindow.h"
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setWindowTitle("Catatan");
	resize(1280, 720);

	// TOOLBAR, TOP BAR, BUTTONS
	QToolBar *top_bar = addToolBar("top_bar");
	top_bar->setMovable(false);
	top_bar->setFloatable(false);
	top_bar->setIconSize(QSize(16, 16));

	top_bar->addAction("file");
	top_bar->addAction("undo");
	top_bar->addAction("redo");
	top_bar->addAction("view");
	top_bar->addAction("window");

	// auto *central = new QWidget(this);
	// setCentralWidget(central);
}

MainWindow::~MainWindow()
{
}
