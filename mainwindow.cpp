#include "mainwindow.h"
#include <QTextEdit>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent)
{
		setWindowTitle("Catatan");
		resize(1280, 720);

		setupToolbar();
		newFile();
}

MainWindow::~MainWindow() = default;

// TOOLBAR, TOP BAR, BUTTON
void MainWindow::setupToolbar()
{
		QToolBar *top_bar = addToolBar("top_bar");

		top_bar->setMovable(false);
		top_bar->setFloatable(false);
		top_bar->setIconSize(QSize(16, 16));

		auto *file_action = top_bar->addAction("File");
		file_action->setToolTip("Olah catatan");

		auto *file_button = qobject_cast<QToolButton *>(
				top_bar->widgetForAction(file_action));
		if (file_button) {
				file_button->setPopupMode(QToolButton::InstantPopup);
				setupFileMenu(file_button);
		}
		top_bar->addAction("Undo");
		top_bar->addAction("Redo");
		top_bar->addAction("View");
		top_bar->addAction("Window");
}
void MainWindow::setupFileMenu(QToolButton *parent_button)
{
		auto *file_menu = new QMenu(parent_button);
		file_menu->addAction("New File", []() { qDebug() << "New File"; });
		file_menu->addAction("New Folder", []() { qDebug() << "New Folder"; });
		file_menu->addSeparator();
		file_menu->addAction("Open File", []() { qDebug() << "Open File"; });
		file_menu->addAction("Open Folder",
							 []() { qDebug() << "Open Folder"; });

		parent_button->setMenu(file_menu);
}
void MainWindow::newFile()
{
		if (!m_text_edit) {
				// First time: create editor + layout
				m_text_edit = new QTextEdit(this);
				m_text_edit->setFrameShape(QFrame::NoFrame); // remove border
				m_text_edit->setStyleSheet("background: transparent;");

				auto *central = new QWidget(this);
				auto *layout = new QVBoxLayout(central);
				layout->setContentsMargins(0, 0, 0, 0);
				layout->addWidget(m_text_edit);
				setCentralWidget(central);
		}
		m_text_edit->clear(); // just clear content
		m_text_edit->setPlainText(""); // ensure empty
}
