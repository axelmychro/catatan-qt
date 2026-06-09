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
#include <QTreeView>
#include <QFileSystemModel>
#include <QDockWidget>
#include <QTextDocument>
#include <QTextBrowser>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextBlock>
// INCLUDE TAMBAHAN
#include <QClipboard>
#include <QMimeData>
#include <QDateTime>
#include <QApplication>

// SUB-CLASS KHUSUS: Mengajari QTextEdit agar bisa mengenali Paste Gambar secara otomatis
class CustomTextEdit : public QTextEdit {
	private:
		MainWindow* m_mainWindow;

	protected:
		void insertFromMimeData(const QMimeData* source) override {
				// Cek apakah data yang di-paste mengandung gambar/screenshot
				if (source && source->hasImage()) {
						m_mainWindow->handleImagePaste();
						return;
				}
				// Jika teks biasa, biarkan berjalan normal seperti biasa
				QTextEdit::insertFromMimeData(source);
		}

	public:
		CustomTextEdit(MainWindow* mainWindow, QWidget* parent = nullptr)
				: QTextEdit(parent), m_mainWindow(mainWindow) {}
};

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

		auto *undo_action = top_bar->addAction("Undo");
		undo_action->setToolTip("Undo (Ctrl+Z)");
		undo_action->setShortcut(QKeySequence::Undo);
		connect(undo_action, &QAction::triggered, this, &MainWindow::undo);

		auto *redo_action = top_bar->addAction("Redo");
		redo_action->setToolTip("Redo (Ctrl+Y)");
		redo_action->setShortcut(QKeySequence::Redo);
		connect(redo_action, &QAction::triggered, this, &MainWindow::redo);

		m_preview_action = top_bar->addAction("Preview");
		m_preview_action->setToolTip("Preview Markdown");
		m_preview_action->setEnabled(false);
		connect(m_preview_action, &QAction::triggered, this,
				&MainWindow::showPreview);

		// ATTACHMENT
		m_attach_action = top_bar->addAction("Attach Image");
		m_attach_action->setToolTip(
				"Insert image into document (Ctrl+Shift+I)");
		m_attach_action->setShortcut(QKeySequence("Ctrl+Shift+I"));
		m_attach_action->setEnabled(false);
		connect(m_attach_action, &QAction::triggered, this,
				&MainWindow::attachImage);

		top_bar->addAction("Window");
}

void MainWindow::setupFileMenu(QToolButton *parent_button)
{
		auto *file_menu = new QMenu(parent_button);
		file_menu->addAction("New File", this, &MainWindow::newFile);
		file_menu->addAction("Open File", this, &MainWindow::openFile);
		file_menu->addAction("Open Folder", this, &MainWindow::openFolder);
		file_menu->addSeparator();
		file_menu->addAction("Save File", this, &MainWindow::saveFile);
		file_menu->addAction("Close File", this, &MainWindow::closeFile);
		parent_button->setMenu(file_menu);
}

void MainWindow::newFile()
{
		m_current_path.clear();
		checkPreviewAvailability();

		if (m_text_edit) {
				m_text_edit->clear();
				m_text_edit->setFocus();
				updateStatus("untitled");
				if (m_pos_label)
						m_pos_label->setText("1:1");
				return;
		}

		if (m_welcome)
				m_welcome->hide();

		// MENGGUNAKAN CUSTOM TEXT EDIT YANG SUDAH KITA AJARI FITUR PASTE GAMBAR
		m_text_edit = new CustomTextEdit(this, this);
		m_text_edit->setFrameShape(QFrame::NoFrame);

		connect(m_text_edit, &QTextEdit::cursorPositionChanged, this, [this]() {
				if (!m_pos_label || !m_text_edit)
						return;
				auto cursor = m_text_edit->textCursor();
				m_pos_label->setText(QString("%1:%2")
											 .arg(cursor.blockNumber() + 1)
											 .arg(cursor.columnNumber() + 1));
		});

		auto *central = new QWidget(this);
		auto *layout = new QVBoxLayout(central);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(m_text_edit);
		setCentralWidget(central);
		m_welcome = nullptr;

		m_text_edit->setFocus();
		updateStatus("untitled");
		if (m_pos_label)
				m_pos_label->setText("1:1");
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
		m_current_path = path;
		checkPreviewAvailability();
		updateStatus(path);
}

void MainWindow::openFolder()
{
		QString path = QFileDialog::getExistingDirectory(this, "Open Folder",
														 QDir::homePath());
		if (path.isEmpty())
				return;

		if (!m_folder_dock)
				setupFolderSidebar();

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
		m_folder_view->hideColumn(1);
		m_folder_view->hideColumn(2);
		m_folder_view->hideColumn(3);

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
								checkPreviewAvailability();

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
		m_text_edit = nullptr;

		m_welcome = new QLabel("Catatan", this);
		m_welcome->setAlignment(Qt::AlignCenter);
		m_welcome->setObjectName("welcomeLabel");

		setCentralWidget(m_welcome);

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
		if (!m_text_edit || m_text_edit->toPlainText().isEmpty())
				return;

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
		checkPreviewAvailability(); // Biar tombol langsung mendeteksi setelah save pertama kali
}

void MainWindow::closeFile()
{
		m_current_path.clear();
		checkPreviewAvailability();
		showWelcome();
}

void MainWindow::undo()
{
		if (m_text_edit)
				m_text_edit->undo();
}

void MainWindow::redo()
{
		if (m_text_edit)
				m_text_edit->redo();
}

// LOGIKA ASLI KAMU TETAP DIJAGA UTUH
void MainWindow::checkPreviewAvailability()
{
		if (m_preview_action) {
				bool isMd =
						m_current_path.endsWith(".md", Qt::CaseInsensitive) ||
						m_current_path.endsWith(".markdown",
												Qt::CaseInsensitive);
				m_preview_action->setEnabled(isMd && m_text_edit != nullptr);
		}

		if (m_attach_action)
				m_attach_action->setEnabled(m_text_edit != nullptr);
}

void MainWindow::attachImage()
{
		if (!m_text_edit)
				return;

		QString src = QFileDialog::getOpenFileName(
				this, "Attach Image", QDir::homePath(),
				"Images (*.png *.jpg *.jpeg *.gif *.webp *.bmp *.svg);;All Files (*)");
		if (src.isEmpty())
				return;

		QString mdPath;

		if (!m_current_path.isEmpty()) {
				QFileInfo docInfo(m_current_path);
				QDir attachDir(docInfo.dir().filePath("_attachments"));
				if (!attachDir.exists())
						attachDir.mkpath(".");

				QFileInfo imgInfo(src);
				QString destName = imgInfo.fileName();
				QString destPath = attachDir.filePath(destName);

				if (QFile::exists(destPath) && destPath != src) {
						QString base = imgInfo.completeBaseName();
						QString ext = imgInfo.suffix();
						int n = 1;
						while (QFile::exists(destPath)) {
								destName = QString("%1_%2.%3")
								.arg(base)
										.arg(n++)
										.arg(ext);
								destPath = attachDir.filePath(destName);
						}
				}

				if (src != destPath)
						QFile::copy(src, destPath);

				mdPath = QString("_attachments/%1").arg(destName);
		} else {
				mdPath = src;
		}

		QFileInfo imgInfo(src);
		QString altText = imgInfo.completeBaseName();
		QString snippet = QString("![%1](%2)").arg(altText, mdPath);

		QTextCursor cursor = m_text_edit->textCursor();
		if (!cursor.atBlockStart() && cursor.block().text().length() > 0)
				snippet.prepend("\n");
		if (!cursor.atBlockEnd() && cursor.block().text().length() > 0)
				snippet.append("\n");

		cursor.insertText(snippet);
		m_text_edit->setTextCursor(cursor);
}

// LOGIKA PROSES SIMPAN GAMBAR DARI CLIPBOARD
void MainWindow::handleImagePaste()
{
		if (!m_text_edit)
				return;

		const QClipboard *clipboard = QApplication::clipboard();
		const QMimeData *mimeData = clipboard->mimeData();

		if (mimeData->hasImage()) {
				QImage img = qvariant_cast<QImage>(mimeData->imageData());
				if (img.isNull())
						return;

				QString mdPath;
				QString destPath;
				QString fileName = QString("pasted_%1.png").arg(QDateTime::currentMSecsSinceEpoch());

				if (!m_current_path.isEmpty()) {
						QFileInfo docInfo(m_current_path);
						QDir attachDir(docInfo.dir().filePath("_attachments"));
						if (!attachDir.exists())
								attachDir.mkpath(".");

						destPath = attachDir.filePath(fileName);
						mdPath = QString("_attachments/%1").arg(fileName);
				} else {
						// Jika masih untitled, simpan di folder Temp sistem laptop
						destPath = QDir::temp().filePath(fileName);
						mdPath = QUrl::fromLocalFile(destPath).toString();
				}

				if (img.save(destPath, "PNG")) {
						QString snippet = QString("![Pasted Image](%2)").arg(mdPath);

						QTextCursor cursor = m_text_edit->textCursor();
						if (!cursor.atBlockStart() && cursor.block().text().length() > 0)
								snippet.prepend("\n");
						if (!cursor.atBlockEnd() && cursor.block().text().length() > 0)
								snippet.append("\n");

						cursor.insertText(snippet);
						m_text_edit->setTextCursor(cursor);
				}
		}
}

void MainWindow::showPreview()
{
		if (!m_text_edit || m_current_path.isEmpty())
				return;

		QDialog *previewDialog = new QDialog(this);
		previewDialog->setWindowTitle("Preview: " + QFileInfo(m_current_path).fileName());
		previewDialog->resize(900, 700);

		QTextBrowser *browser = new QTextBrowser(previewDialog);
		browser->setOpenExternalLinks(true);

		QFileInfo docInfo(m_current_path);
		browser->setSearchPaths(QStringList() << docInfo.absolutePath());

		QVBoxLayout *layout = new QVBoxLayout(previewDialog);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(browser);
		previewDialog->setLayout(layout);

		QTextDocument doc;
		doc.setMarkdown(m_text_edit->toPlainText());
		doc.setBaseUrl(QUrl::fromLocalFile(docInfo.absolutePath() + "/"));

		QString html = doc.toHtml();

		QString css =
				"<style>"
				"body { font-family: sans-serif; color: #cad3f5; background: #24273a; padding: 16px; line-height: 1; }"
				"h1, h2, h3, h4, h5, h6 { color: #f5bde6; margin-top: 16px; margin-bottom: 16px; font-weight: 600; }"
				"h1 { font-size: 2em; }"
				"h2 { font-size: 1.5em; }"
				"p { margin-bottom: 16px; }"
				"code { background: #1e2030; padding: 2px 2px; border-radius: 8px; font-family: monospace; color: #a6da95; }"
				"pre { background: #11111b; padding: 16px; border-radius: 8px; overflow-x: auto; border: 1px solid #313244; }"
				"pre code { background: transparent; color: #cdd6f4; padding: 0; }"
				"blockquote { border-left: 4px solid #89b4fa; margin: 16px 0; padding: 8px 16px; background: #313244; color: #a6adc8; border-radius: 0 4px 4px 0; }"
				"ul, ol { margin-bottom: 16px; padding-left: 32px; }"
				"li { margin-bottom: 8px; }"
				"li input[type='checkbox'] { margin-right: 8px; transform: scale(1.2); accent-color: #89b4fa; }"
				"hr { border: 0; border-top: 2px solid #313244; margin: 24px 0; }"
				"a { color: #89b4fa; text-decoration: none; }"
				"a:hover { text-decoration: underline; }"
				"table { border-collapse: collapse; width: 100%; margin-bottom: 16px; }"
				"th, td { border: 1px solid #313244; padding: 8px 12px; text-align: left; }"
				"th { background: #313244; color: #89b4fa; }"
				"strong { color: #f5c2e7; font-weight: bold; }"
				"em { color: #a6e3a1; font-style: italic; }"
				"del { color: #6c7086; text-decoration: line-through; }"
				"img { max-width: 100%; border-radius: 8px; margin: 16px 0; }"
				"</style>";

		browser->setHtml(css + html);
		previewDialog->exec();
		previewDialog->deleteLater();
}
// end mainwindow.cpp