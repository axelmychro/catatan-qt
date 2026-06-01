#include "mainwindow.h"
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setWindowTitle("Catatan");
	resize(1280, 720);

	// TOOLBAR, TOP BAR, BUTTON
	QToolBar *top_bar = addToolBar("top_bar");
	top_bar->setMovable(false);
	top_bar->setFloatable(false);
	top_bar->setIconSize(QSize(16, 16));

	auto *file_action = top_bar->addAction("File");
	file_action->setToolTip("Olah catatan");

	top_bar->addAction("Undo");
	top_bar->addAction("Redo");
	top_bar->addAction("View");
	top_bar->addAction("Window");

	QList<QToolButton *> tool_buttons =
		top_bar->findChildren<QToolButton *>();
	for (QToolButton *btn : tool_buttons) {
		if (btn->text() == "File") {
			btn->setPopupMode(QToolButton::InstantPopup);

			QMenu *file_menu = new QMenu(btn);
			file_menu->addAction("New File",
					     []() { qDebug() << "New File"; });
			file_menu->addAction("New Folder", []() {
				qDebug() << "New Folder";
			});
			file_menu->addSeparator();
			file_menu->addAction("Open File",
					     []() { qDebug() << "Open File"; });
			file_menu->addAction("Open Folder", []() {
				qDebug() << "Open Folder";
			});

			btn->setMenu(file_menu);
			break;
		}
	}
}

MainWindow::~MainWindow()
{
}
