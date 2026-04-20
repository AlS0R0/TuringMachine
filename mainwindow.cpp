#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "simulatorwindow.h"

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
    //SimulatorWindow *sim = new SimulatorWindow(this);
    //sim->setAttribute(Qt::WA_DeleteOnClose);
    //sim->show();

    //hide();
    //this->close();

    SimulatorWindow simWindow(this);
    simWindow.setModal(true);
    simWindow.exec();
}
