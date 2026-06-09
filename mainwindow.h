#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QTextEdit>
#include <QFileInfo>
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

		// ✨ SOLUSI PASTI: Kita jadikan PUBLIC agar kelas CustomTextEdit di mainwindow.cpp bebas memanggilnya tanpa eror privasi kontekstual!
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
		QLabel *m_file_label = nullptr;
		QLabel *m_pos_label = nullptr;
		QLabel *m_welcome = nullptr;

		QAction *m_preview_action = nullptr;
		QAction *m_attach_action = nullptr;

		QDockWidget *m_folder_dock = nullptr;
		QTreeView *m_folder_view = nullptr;
		QFileSystemModel *m_folder_model = nullptr;
};
#endif // MAINWINDOW_H