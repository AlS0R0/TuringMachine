#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "simulatorwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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

    if (text1.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Алфавит не может быть пустым");
        return;
    }

    QStringList symbols = (text1 + " " + text2).split(' ', Qt::SkipEmptyParts);
    QSet<QChar> alphabet;

    for (int i = 0; i < symbols.size(); ++i) {
        const QString& s = symbols[i];
        if (!s.isEmpty()) alphabet.insert(s[0]);
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

    SimulatorWindow simWindow(this);
    simWindow.setModal(true);
    simWindow.exec();
}
