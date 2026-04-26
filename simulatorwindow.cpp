#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"
#include "conditiontable.h"
#include "tapewidget.h"
#include "turingmachinekernel.h"
#include <QTableView>

/*
SimulatorWindow::SimulatorWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SimulatorWindow)
{
    ui->setupUi(this);

    ConditionTable *table = new ConditionTable(10, 10, this);
    QModelIndex index = table->index(2, 3);
    table->setData(index, "^", Qt::EditRole);

    QTableView *tableView = new QTableView();
    tableView->setModel(table);

    ui->gridLayout_5->addWidget(tableView);

}
*/

SimulatorWindow::SimulatorWindow(QString str1, QString str2, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SimulatorWindow)
{
    ui->setupUi(this);

    m_kernel = new TuringMachineKernel(this);

    m_tapeWidget = new TapeWidget(this);
    m_tapeWidget->setKernel(m_kernel); // ядро нужно создать или передать
    ui->verticalLayout->insertWidget(0, m_tapeWidget); // или заменить существующий QGraphicsView

    connect(ui->AddCondition, &QPushButton::clicked, this, &SimulatorWindow::AddCondition_clicked);
    connect(ui->RemoveCondition, &QPushButton::clicked, this, &SimulatorWindow::RemoveCondition_clicked);
    connect(ui->ChangeAlphabet, &QPushButton::clicked, this, &SimulatorWindow::ChangeAlphabet_clicked);

    this->model = new ConditionTable(1, str1, str2, this);

    this->tableView = new QTableView();
    tableView->setModel(this->model);

    ui->gridLayout_5->addWidget(tableView);

}

SimulatorWindow::~SimulatorWindow()
{
    delete ui;
}

void SimulatorWindow::AddCondition_clicked()
{
    if (model)
        this->model->addEmptyRow();
}

void SimulatorWindow::RemoveCondition_clicked()
{
    if (model)
        this->model->removeRow();
}

void SimulatorWindow::ChangeAlphabet_clicked()
{
    hide();
    this->model->addEmptyRow();
}
