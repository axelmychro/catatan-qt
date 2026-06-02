#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QTextEdit>
#include <QFileInfo>
#include <QLabel>

class QTextEdit;

class MainWindow : public QMainWindow {
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow() override;

		// TOOLBAR, TOP BAR, BUTTOn
	private:
		void setupToolbar();
		void setupFileMenu(QToolButton *parent_button);
		void setupStatusBar();
		void showWelcome();

		void newFile();
		void openFile();
		void updateStatus(const QString &file_name = {});

		void saveFile();
		void closeFile();

		QString m_current_path;

		QTextEdit *m_text_edit = nullptr;
		QStatusBar *m_status_bar = nullptr;
		QLabel *m_file_label = nullptr;
		QLabel *m_pos_label = nullptr;
		QLabel *m_welcome = nullptr;
};
#endif // MAINWINDOW_H
