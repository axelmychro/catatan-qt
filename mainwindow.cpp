// start mainwindow.cpp
#include "mainwindow.h"
#include <QTextEdit>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent)
{
		setWindowTitle("Catatan");
		resize(1280, 720);

		setupToolbar();
		setupStatusBar();
		showWelcome();
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

		file_menu->addAction("New File", this, &MainWindow::newFile);
		file_menu->addAction("Open File", this, &MainWindow::openFile);
		file_menu->addAction("Open Folder", this,
							 &MainWindow::openFolder); // ← wire it
		file_menu->addSeparator();
		file_menu->addAction("Save File", this, &MainWindow::saveFile);
		file_menu->addAction("Close File", this, &MainWindow::closeFile);

		parent_button->setMenu(file_menu);
}
void MainWindow::newFile()
{
		m_current_path.clear();
		m_welcome->hide();

		if (!m_text_edit) {
				m_text_edit = new QTextEdit(this);
				m_text_edit->setFrameShape(QFrame::NoFrame);
				m_text_edit->setStyleSheet("background: transparent;");

				auto *central = new QWidget(this);
				auto *layout = new QVBoxLayout(central);
				layout->setContentsMargins(0, 0, 0, 0);
				layout->addWidget(m_text_edit);
				setCentralWidget(central);
		}

		m_text_edit->clear();
		m_text_edit->setPlainText("");
}
void MainWindow::openFile()
{
		QString path = QFileDialog::getOpenFileName(
				this, "Open File", QDir::homePath(),
				"Text Files (*.txt *.md);;All Files (*)");
		if (path.isEmpty())
				return;

		QFile file(path);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
				QMessageBox::warning(this, "Error", "Cannot open:\n" + path);
				return;
		}

		if (!m_text_edit)
				newFile();

		m_text_edit->setPlainText(QTextStream(&file).readAll());

		m_current_path = path; // ← track path after open
		updateStatus(path);
}

void MainWindow::openFolder()
{
		QString path = QFileDialog::getExistingDirectory(this, "Open Folder",
														 QDir::homePath());
		if (path.isEmpty())
				return;

		if (!m_folder_dock) {
				setupFolderSidebar();
		}

		m_folder_model->setRootPath(path);
		m_folder_view->setRootIndex(m_folder_model->index(path));

		m_folder_dock->show();

		updateStatus(path + "/");
}

void MainWindow::setupFolderSidebar()
{
		m_folder_model = new QFileSystemModel(this);
		m_folder_model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
		m_folder_model->setNameFilters(QStringList()
									   << "*.txt" << "*.md" << "*");
		m_folder_model->setNameFilterDisables(false);

		m_folder_view = new QTreeView(this);
		m_folder_view->setModel(m_folder_model);
		m_folder_view->setHeaderHidden(true);
		m_folder_view->setStyleSheet(
				"background: transparent; color: {{fg}}; border: none;"
				"QTreeView::item { padding: 2px; }"
				"QTreeView::item:selected { background: {{active}}; }");

		m_folder_view->hideColumn(1); // size
		m_folder_view->hideColumn(2); // type
		m_folder_view->hideColumn(3); // modified

		connect(m_folder_view, &QTreeView::doubleClicked, this,
				[this](const QModelIndex &index) {
						if (!m_folder_model || !index.isValid())
								return;

						QString path = m_folder_model->filePath(index);
						QFileInfo info(path);

						if (info.isFile()) {
								if (!m_text_edit)
										newFile();
								m_current_path = path;

								QFile file(path);
								if (file.open(QIODevice::ReadOnly |
											  QIODevice::Text)) {
										m_text_edit->setPlainText(
												QTextStream(&file).readAll());
										updateStatus(path);
								}
						}
				});

		m_folder_dock = new QDockWidget("Files", this);
		m_folder_dock->setObjectName("folderDock");
		m_folder_dock->setWidget(m_folder_view);
		m_folder_dock->setFeatures(QDockWidget::DockWidgetMovable |
								   QDockWidget::DockWidgetClosable);
		m_folder_dock->setAllowedAreas(Qt::LeftDockWidgetArea |
									   Qt::RightDockWidgetArea);

		addDockWidget(Qt::LeftDockWidgetArea, m_folder_dock);

		m_folder_dock->hide();
}
void MainWindow::setupStatusBar()
{
		m_status_bar = statusBar();
		m_status_bar->setSizeGripEnabled(false);

		m_file_label = new QLabel("untitled", this);
		m_file_label->setStyleSheet("color: {{fg}}; margin-left: 4px;");

		m_pos_label = new QLabel("1:1", this);
		m_pos_label->setStyleSheet("color: {{fg}}; margin-right: 4px;");

		m_status_bar->addWidget(m_file_label, 1);
		m_status_bar->addPermanentWidget(m_pos_label);
}

void MainWindow::showWelcome()
{
		if (m_text_edit)
				m_text_edit->hide();

		if (!m_welcome) {
				m_welcome = new QLabel("Catatan", this);
				m_welcome->setAlignment(Qt::AlignCenter);
				m_welcome->setStyleSheet(
						"font-size: 32px; color: {{fg}}; background: transparent;");
				m_welcome->setObjectName("welcomeLabel");
		}

		setCentralWidget(m_welcome);
		m_welcome->show();

		updateStatus("untitled");
}

void MainWindow::updateStatus(const QString &fileName)
{
		if (m_file_label) {
				m_file_label->setText(fileName.isEmpty() ?
											  "untitled" :
											  QFileInfo(fileName).fileName());
		}
}

void MainWindow::saveFile()
{
		if (!m_text_edit || m_text_edit->toPlainText().isEmpty()) {
				return;
		}
		if (m_current_path.isEmpty()) {
				QString path = QFileDialog::getSaveFileName(
						this, "Save File", QDir::homePath(),
						"Text Files (*.txt *.md);;All Files (*)");
				if (path.isEmpty())
						return;
				m_current_path = path;
		}
		QFile file(m_current_path);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QMessageBox::warning(this, "Error",
									 "Cannot save:\n" + m_current_path);
				return;
		}
		QTextStream out(&file);
		out << m_text_edit->toPlainText();

		updateStatus(m_current_path);
}

void MainWindow::closeFile()
{
		m_current_path.clear();
		//showWelcome();
}
// end mainwindow.cpp
