#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QTextCursor>

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

void MainWindow::on_btnAttachment_clicked()
{

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Pilih File untuk Dilampirkan"),
        QDir::homePath(),
        tr("Semua File (*.*);;Gambar (*.png *.jpg);;Dokumen (*.pdf *.txt)")
        );

    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        QTextCursor cursor = ui->textEdit->textCursor();
        QString attachmentHtml = QString("<br><a href='%1' style='color: #00bcff; text-decoration: underline;'>"
                                         "📎 [Attachment: %2]</a><br>")
                                     .arg(filePath)
                                     .arg(fileName);

        cursor.insertHtml(attachmentHtml);
    }
}