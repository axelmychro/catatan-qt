#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QTextEdit>

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

		void newFile();
		void openFile();

		QTextEdit *m_text_edit = nullptr;
};
#endif // MAINWINDOW_H
