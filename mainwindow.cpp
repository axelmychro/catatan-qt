
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDateTime>
#include <QColorDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QAction>
#include <QTextCursor>

    MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString waktuSekarang =
        QDateTime::currentDateTime()
            .toString("dd MMMM yyyy, hh.mm");

    ui->label->setText(waktuSekarang.toUpper());

    ui->lineEdit->setPlaceholderText("Judul");

    ui->textEdit->setPlaceholderText("Ketik di sini...");

    this->setStyleSheet(R"(

        QMainWindow {
            background-color: #FDF6E3;
        }

        QLabel#label {
            color: #795548;
            font-size: 11px;
            font-weight: bold;
        }

        QLineEdit#lineEdit {
            background: transparent;
            border: none;
            color: #5D4037;
            font-size: 24px;
            font-weight: bold;
        }

        QTextEdit#textEdit {
            background: transparent;
            border: none;
            color: #5D4037;
            font-size: 15px;
            line-height: 1.6;
        }

        QPushButton#pushButton {
            background-color: #4A3525;
            color: #FDF6E3;
            border-radius: 15px;
            font-weight: bold;
            padding: 5px 15px;
        }

        QPushButton#pushButton:hover {
            background-color: #5D4037;
        }

        QPushButton#pushButton_2 {
            background: transparent;
            border: none;
            color: #4A3525;
            font-weight: bold;
        }

    )");

    QAction *shortcutBold = new QAction(this);

    shortcutBold->setShortcut(QKeySequence("Ctrl+B"));

    connect(shortcutBold,
            &QAction::triggered,
            this,
            [this]() {

                QTextCharFormat format;

                format.setFontWeight(
                    ui->textEdit->fontWeight() == QFont::Bold
                        ? QFont::Normal
                        : QFont::Bold
                    );

                ui->textEdit->mergeCurrentCharFormat(format);
            });

    this->addAction(shortcutBold);

    QAction *shortcutItalic = new QAction(this);

    shortcutItalic->setShortcut(QKeySequence("Ctrl+I"));

    connect(shortcutItalic,
            &QAction::triggered,
            this,
            [this]() {

                QTextCharFormat format;

                format.setFontItalic(
                    !ui->textEdit->fontItalic()
                    );

                ui->textEdit->mergeCurrentCharFormat(format);
            });

    this->addAction(shortcutItalic);

    connect(ui->pushButton,
            &QPushButton::clicked,
            this,
            &MainWindow::simpanFile);

    connect(ui->pushButton_2,
            &QPushButton::clicked,
            this,
            &MainWindow::ubahWarnaBackground);

    // Markdown checklist sederhana
    connect(ui->textEdit,
            &QTextEdit::textChanged,
            this,
            [this]() {

                QTextCursor cursor =
                    ui->textEdit->textCursor();

                QString text =
                    ui->textEdit->toPlainText();

                QString newText = text;

                newText.replace("- [ ] ", "☐ ");
                newText.replace("- [x] ", "☑ ");

                if (newText != text) {

                    int pos = cursor.position();

                    ui->textEdit->blockSignals(true);

                    ui->textEdit->setPlainText(newText);

                    cursor.setPosition(pos);

                    ui->textEdit->setTextCursor(cursor);

                    ui->textEdit->blockSignals(false);
                }
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj,
                             QEvent *event)
{
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::ubahWarnaBackground()
{
    QColor warnaDipilih =
        QColorDialog::getColor(
            Qt::white,
            this,
            "Pilih Warna Background"
            );

    if (warnaDipilih.isValid()) {

        QString style =
            "QMainWindow { background-color: "
            + warnaDipilih.name() +
            "; }"

            "QLabel#label {"
            " color: #795548;"
            " font-size: 11px;"
            " font-weight: bold;"
            "}"

            "QLineEdit#lineEdit {"
            " background: transparent;"
            " border: none;"
            " color: #5D4037;"
            " font-size: 24px;"
            " font-weight: bold;"
            "}"

            "QTextEdit#textEdit {"
            " background: transparent;"
            " border: none;"
            " color: #5D4037;"
            " font-size: 15px;"
            " line-height: 1.6;"
            "}";

        this->setStyleSheet(style);
    }
}

void MainWindow::simpanFile()
{
    QString namaFile =
        QFileDialog::getSaveFileName(
            this,
            "Simpan Catatan Anda",
            "",
            "Text Files (*.txt)"
            );

    if (namaFile.isEmpty()) {
        return;
    }

    QFile file(namaFile);

    if (!file.open(QIODevice::WriteOnly
                   | QIODevice::Text)) {

        QMessageBox::critical(
            this,
            "Error",
            "Gagal menyimpan file!"
            );

        return;
    }

    QTextStream out(&file);

    out << "# "
        << ui->lineEdit->text()
        << "\n\n";

    out << ui->textEdit->toPlainText();

    file.close();

    QMessageBox::information(
        this,
        "Sukses",
        "Catatan sukses disimpan!"
        );
}
