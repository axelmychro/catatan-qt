#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWebEngineView>
#include <QMainWindow>
#include <QToolButton>
#include <QTextEdit>
#include <QLabel>
#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QAction>

class MainWindow : public QMainWindow {
		Q_OBJECT
	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow() override;
		void handleImagePaste();
	private slots:
		void undo();
		void redo();
		void showPreview();

	private:
		void setupToolbar();
		void setupFileMenu(QToolButton *parent_button);
		void setupStatusBar();
		void showWelcome();
		void checkPreviewAvailability();
		void updateStatus(const QString &file_name = {});
		void newFile();
		void openFile();
		void openFolder();
		void setupFolderSidebar();
		void saveFile();
		void closeFile();
		void attachImage();
		QString m_current_path;
		QTextEdit *m_text_edit = nullptr;
		QStatusBar *m_status_bar = nullptr;
		QLabel *m_file_label = nullptr, *m_pos_label = nullptr,
			   *m_welcome = nullptr;
		QAction *m_preview_action = nullptr, *m_attach_action = nullptr;
		QWebEngineView *m_web_view = nullptr;
		QDockWidget *m_folder_dock = nullptr;
		QTreeView *m_folder_view = nullptr;
		QFileSystemModel *m_folder_model = nullptr;
};
#endif
