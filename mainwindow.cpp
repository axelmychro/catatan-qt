#include "mainwindow.h"
#include "mainwindow.moc"
#include <QWebEngineView>
#include <QWebEnginePage>
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
#include <QClipboard>
#include <QMimeData>
#include <QDateTime>
#include <QApplication>

class CustomTextEdit : public QTextEdit {
	private:
		MainWindow *m_mainWindow;

	protected:
		void insertFromMimeData(const QMimeData *source) override
		{
				if (source && source->hasImage()) {
						m_mainWindow->handleImagePaste();
						return;
				}
				QTextEdit::insertFromMimeData(source);
		}

	public:
		CustomTextEdit(MainWindow *mainWindow, QWidget *parent = nullptr)
				: QTextEdit(parent)
				, m_mainWindow(mainWindow)
		{
		}
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
		auto *file_button = qobject_cast<QToolButton *>(
				top_bar->widgetForAction(file_action));
		if (file_button) {
				file_button->setPopupMode(QToolButton::InstantPopup);
				setupFileMenu(file_button);
		}
		auto *undo_action = top_bar->addAction("Undo");
		undo_action->setShortcut(QKeySequence::Undo);
		connect(undo_action, &QAction::triggered, this, &MainWindow::undo);
		auto *redo_action = top_bar->addAction("Redo");
		redo_action->setShortcut(QKeySequence::Redo);
		connect(redo_action, &QAction::triggered, this, &MainWindow::redo);
		m_preview_action = top_bar->addAction("Preview");
		m_preview_action->setEnabled(false);
		connect(m_preview_action, &QAction::triggered, this,
				&MainWindow::showPreview);
		m_attach_action = top_bar->addAction("Attach Image");
		m_attach_action->setShortcut(QKeySequence("Ctrl+Shift+I"));
		m_attach_action->setEnabled(false);
		connect(m_attach_action, &QAction::triggered, this,
				&MainWindow::attachImage);
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
				return;
		}
		if (m_welcome)
				m_welcome->hide();
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
}

void MainWindow::openFile()
{
		QString path = QFileDialog::getOpenFileName(
				this, "Open File", QDir::homePath(),
				"Text Files (*.txt *.md);;All Files (*)");
		if (path.isEmpty())
				return;
		QFile file(path);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
				return;
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
		m_folder_view = new QTreeView(this);
		m_folder_view->setModel(m_folder_model);
		connect(m_folder_view, &QTreeView::doubleClicked, this,
				[this](const QModelIndex &index) {
						QString path = m_folder_model->filePath(index);
						if (QFileInfo(path).isFile()) {
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
		m_folder_dock->setWidget(m_folder_view);
		addDockWidget(Qt::LeftDockWidgetArea, m_folder_dock);
		m_folder_dock->hide();
}

void MainWindow::setupStatusBar()
{
		m_status_bar = statusBar();
		m_file_label = new QLabel("untitled", this);
		m_pos_label = new QLabel("1:1", this);
		m_status_bar->addWidget(m_file_label, 1);
		m_status_bar->addPermanentWidget(m_pos_label);
}

void MainWindow::showWelcome()
{
		m_text_edit = nullptr;
		m_welcome = new QLabel("Catatan", this);
		m_welcome->setAlignment(Qt::AlignCenter);
		setCentralWidget(m_welcome);
}

void MainWindow::updateStatus(const QString &fileName)
{
		if (m_file_label)
				m_file_label->setText(fileName.isEmpty() ?
											  "untitled" :
											  QFileInfo(fileName).fileName());
}

void MainWindow::saveFile()
{
		if (!m_text_edit)
				return;
		if (m_current_path.isEmpty()) {
				m_current_path = QFileDialog::getSaveFileName(
						this, "Save File", QDir::homePath(),
						"Text Files (*.md)");
				if (m_current_path.isEmpty())
						return;
		}
		QFile file(m_current_path);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QTextStream(&file) << m_text_edit->toPlainText();
		}
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
void MainWindow::checkPreviewAvailability()
{
		bool isMd = m_current_path.endsWith(".md", Qt::CaseInsensitive);
		if (m_preview_action)
				m_preview_action->setEnabled(isMd && m_text_edit != nullptr);
		if (m_attach_action)
				m_attach_action->setEnabled(m_text_edit != nullptr);
}

void MainWindow::attachImage()
{
		QString src = QFileDialog::getOpenFileName(
				this, "Attach Image", QDir::homePath(), "Images (*.png *.jpg)");
		if (src.isEmpty())
				return;

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

void MainWindow::handleImagePaste()
{
		const QClipboard *clipboard = QApplication::clipboard();
		const QMimeData *mimeData = clipboard->mimeData();
		if (mimeData->hasImage()) {
				QImage img = qvariant_cast<QImage>(mimeData->imageData());
				QString fileName =
						QString("pasted_%1.png")
								.arg(QDateTime::currentMSecsSinceEpoch());
				QString destPath = QDir::temp().filePath(fileName);
				img.save(destPath, "PNG");
				m_text_edit->insertPlainText(
						QString("![Pasted](%1)").arg(destPath));
		}
}

void MainWindow::showPreview()
{
		if (!m_text_edit || m_current_path.isEmpty())
				return;

		QDialog *previewDialog = new QDialog(this);
		previewDialog->setWindowTitle("Preview: " +
									  QFileInfo(m_current_path).fileName());
		previewDialog->resize(900, 700);

		QWebEngineView *webView = new QWebEngineView(previewDialog);

		// Convert Markdown to HTML
		QTextDocument doc;
		doc.setMarkdown(m_text_edit->toPlainText());
		QString htmlContent = doc.toHtml();

		// Inject CSS for resizing and constraint
		// display: block + resize: both + overflow: auto is the standard way to enable browser-native resize
		QString styleAndScript = R"(
        <style>
            body { font-family: sans-serif; color: #cad3f5; background: #24273a; padding: 20px; }
            img { 
                max-height: 400px; 
                display: block;
                resize: both;
                overflow: auto;
                border: 2px dashed #89b4fa;
                padding: 2px;
            }
        </style>
        <script>
            // Optional: force images to be resizable if they don't have it by default
            window.onload = () => {
                document.querySelectorAll('img').forEach(img => {
                    img.style.cursor = 'nwse-resize';
                });
            };
        </script>
    )";

		// Set the base URL so images load correctly relative to the file path
		QUrl baseUrl = QUrl::fromLocalFile(
				QFileInfo(m_current_path).absolutePath() + "/");
		webView->setHtml(styleAndScript + htmlContent, baseUrl);

		QVBoxLayout *layout = new QVBoxLayout(previewDialog);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(webView);
		previewDialog->setLayout(layout);

		previewDialog->exec();
		previewDialog->deleteLater();
}
