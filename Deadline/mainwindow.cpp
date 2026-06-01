#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDate>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addDeadlineButton_clicked()
{
    QString tugas = ui->taskEdit->text();
    QDate deadline = ui->deadlineEdit->date();

    if(tugas.isEmpty())
        return;

    QString data =
        tugas +
        " | Deadline: " +
        deadline.toString("dd/MM/yyyy");

    ui->deadlineList->addItem(data);

    ui->taskEdit->clear();
}

