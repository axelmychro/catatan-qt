#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>

class MainWindow : public QMainWindow {
	Q_OBJECT

    public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

	// TOOLBAR, TOP BAR, BUTTOn
    private slots:
	// void onNewFile();
	// void onNewFolder();
	// void onOpenFile();
	// void onOpenFolder();

    private:
	void setupToolbar();
	void setupFileMenu(QToolButton *parent_button);
};
#endif // MAINWINDOW_H
