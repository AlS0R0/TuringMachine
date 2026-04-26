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

/*
void MainWindow::setAlphabet_clicked() {

    QString text1 = ui->lineEdit_1->text();
    QString text2 = ui->lineEdit_2->text();

    QStringList symbols = (text1 + " " + text2).split(' ', Qt::SkipEmptyParts);

    // if(!this->alphabet.isEmpty()) {
    //     bool flag = 0;

    //     for (int i = 0; i < alphabet.size(); ++i) {
    //         for (int j = 0; j < symbols.size(); ++j) {
    //             if (this->alphabet[i] == symbols[j]) {
    //                 break;
    //             } else if (this->alphabet[i] != symbols[j] && j == symbols.size() - 1) {
    //                 flag = 1;
    //                 break;
    //             }
    //         }

    //         if(flag == 1) {
    //             break;
    //         }
    //     }

    //     if (flag == 0) {

    //     }
    // }

    for (int i = 0; i < symbols.size(); ++i) {
        const QString& s = symbols[i];
        if (!s.isEmpty()) alphabet.push_back(s[0]);
    }

    if (alphabet.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Алфавит не может быть пустым");
        return;
    }


    //SimulatorWindow *sim = new SimulatorWindow(this);
    //sim->setAttribute(Qt::WA_DeleteOnClose);
    //sim->show();

    //hide();
    //this->close();

    SimulatorWindow simWindow(text1, text2, this);
    simWindow.setModal(true);
    simWindow.exec();
} */

void MainWindow::setAlphabet_clicked() {
    QString text1 = ui->lineEdit_1->text();
    QString text2 = ui->lineEdit_2->text();

    alphabet.clear();

    // Собираем все символы без пробелов
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

    // Убираем возможные дубликаты (необязательно, но для порядка)
    QSet<QChar> unique;
    for (const QChar &ch : alphabet) unique.insert(ch);
    alphabet = unique.values().toVector();

    SimulatorWindow simWindow(text1, text2, this);
    simWindow.setModal(true);
    simWindow.exec();
}
