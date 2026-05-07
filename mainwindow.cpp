#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "simulatorwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , alphabet({})
{
    ui->setupUi(this);
    connect(ui->setAlphabet, &QPushButton::clicked, this, &MainWindow::setAlphabet_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setAlphabet_clicked() {
    QString text1 = ui->lineEdit_1->text();
    QString text2 = ui->lineEdit_2->text();

    alphabet.clear();

    auto addChars = [&](const QString &s) {
        for (const QChar &ch : s) {
            if (!ch.isSpace()) {
                alphabet.append(ch);
            }
        }
    };

    addChars(text1);
    addChars(text2);

    if (alphabet.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Алфавит не может быть пустым");
        return;
    }

    QSet<QChar> unique;
    for (const QChar &ch : alphabet) unique.insert(ch);
    alphabet = unique.values().toVector();

    SimulatorWindow simWindow(alphabet, this);
    simWindow.setModal(true);
    simWindow.exec();
}
