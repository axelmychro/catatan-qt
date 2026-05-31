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

	top_bar->addAction("File");
	top_bar->addAction("Undo");
	top_bar->addAction("Redo");
	top_bar->addAction("View");
	top_bar->addAction("Window");

	// auto *central = new QWidget(this);
	// setCentralWidget(central);
}

MainWindow::~MainWindow()
{
}
