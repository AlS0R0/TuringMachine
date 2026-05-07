#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"
#include "conditiontable.h"
#include "tapewidget.h"
#include "turingmachinekernel.h"
#include "changealphabetforturingmachine.h"
#include <QTableView>
#include <QMessageBox>


SimulatorWindow::SimulatorWindow(QVector<QChar> vec, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SimulatorWindow)
{
    ui->setupUi(this);

    kernel = new TuringMachineKernel();

    m_tapeWidget = new TapeWidget(this);
    m_tapeWidget->setKernel(kernel);

    ui->verticalLayout->addWidget(m_tapeWidget);

    Timer = new QTimer;
    Timer->setInterval(500);
    connect(Timer, &QTimer::timeout, this, &SimulatorWindow::StepMachine_clicked);

    connect(ui->AddCondition, &QPushButton::clicked, this, &SimulatorWindow::AddCondition_clicked);
    connect(ui->RemoveCondition, &QPushButton::clicked, this, &SimulatorWindow::RemoveCondition_clicked);
    connect(ui->ChangeAlphabet, &QPushButton::clicked, this, &SimulatorWindow::ChangeAlphabet_clicked);
    connect(ui->SetLine, &QPushButton::clicked, this, &SimulatorWindow::SetLine_clicked);
    connect(ui->StartMachine, &QPushButton::clicked, this, &SimulatorWindow::StartMachine_clicked);
    connect(ui->StepMachine, &QPushButton::clicked, this, &SimulatorWindow::StepMachine_clicked);
    connect(ui->ContinueButton, &QPushButton::clicked, this, &SimulatorWindow::ContinueButton_clicked);
    connect(ui->PauseMachine, &QPushButton::clicked, this, &SimulatorWindow::PauseMachine_clicked);
    connect(ui->StopMachine, &QPushButton::clicked, this, &SimulatorWindow::StopMachine_clicked);
    connect(ui->ResetLine, &QPushButton::clicked, this, &SimulatorWindow::ResetLine_clicked);
    connect(ui->IncSpeed, &QPushButton::clicked, this, &SimulatorWindow::IncSpeed_clicked);
    connect(ui->DecSpeed, &QPushButton::clicked, this, &SimulatorWindow::DecSpeed_clicked);


    this->model = new ConditionTable(1, vec, this);

    this->tableView = new QTableView();
    tableView->setModel(this->model);

    ui->gridLayout_5->addWidget(tableView);

}

SimulatorWindow::~SimulatorWindow()
{
    delete ui;
}

void SimulatorWindow::setControlsEnabled(bool enabled)
{
    ui->LineTape->setReadOnly(!enabled);
    ui->SetLine->setEnabled(enabled);
    ui->AddCondition->setEnabled(enabled);
    ui->RemoveCondition->setEnabled(enabled);
    ui->ChangeAlphabet->setEnabled(enabled);
    ui->StartMachine->setEnabled(enabled);
    ui->ContinueButton->setEnabled(enabled);
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
    setControlsEnabled(false);

    QVector<QChar> new_alphabet;

    ChangeAlphabetForTuringMachine changeWindow(this, &new_alphabet);
    changeWindow.setModal(true);
    changeWindow.exec();

    setControlsEnabled(true);

    model->updateAlphabet(new_alphabet);

}

void SimulatorWindow::SetLine_clicked()
{
    QString input = ui->LineTape->text();

    for (int i = 0; i < input.size(); ++i) {
        if (!model->getColumnHeaders().contains(input[i]) || input[i] == '^') {
            QMessageBox::warning(this, "Стоп", "Входящая строка содержит неприемлемые символы");
            return;
        }
    }

    kernel->setInputString(input);
    initialInput_ = input;
    m_tapeWidget->setKernel(kernel);
}

void SimulatorWindow::StartMachine_clicked()
{
    loadRulesFromTable();

    if (Timer) {
        Timer->start();
    }

    setControlsEnabled(false);
}

void SimulatorWindow::StepMachine_clicked() {
    if (!kernel->step()) {
        QMessageBox::information(this, "Стоп", "Машина остановилась");
        PauseMachine_clicked();
        return;
    }

    m_tapeWidget->animateStep();
}

void SimulatorWindow::ContinueButton_clicked() {
    ui->ContinueButton->setEnabled(false);
    Timer->start();
}

void SimulatorWindow::PauseMachine_clicked() {
    ui->ContinueButton->setEnabled(true);
    Timer->stop();
}

void SimulatorWindow::StopMachine_clicked() {
    Timer->stop();
    setControlsEnabled(true);
}

void SimulatorWindow::ResetLine_clicked() {
    Timer->stop();
    kernel->reset(initialInput_);
    m_tapeWidget->setKernel(kernel);
    m_tapeWidget->update();
    setControlsEnabled(true);
}

void SimulatorWindow::IncSpeed_clicked() {
    int currentInterval = Timer->interval();
    if (currentInterval >= 200) Timer->setInterval(currentInterval - 100);
}

void SimulatorWindow::DecSpeed_clicked() {
    int currentInterval = Timer->interval();
    Timer->setInterval(currentInterval + 100);
}

void SimulatorWindow::loadRulesFromTable()
{
    if (!model || !kernel) return;

    QVector<QChar> headers_col = model->getColumnHeaders();
    int rows = model->rowCount(QModelIndex());
    int cols = model->columnCount(QModelIndex());

    for (int i = 0; i < rows; ++i) {

        QString state = "q" + QString::number(i);

        for (int j = 0; j < cols; ++j) {

            QModelIndex idx = model->index(i, j);
            QString value = model->data(idx, Qt::DisplayRole).toString().trimmed();
            if (value.isEmpty()) continue;

            QStringList parts = value.split(',', Qt::SkipEmptyParts);

            QChar direction = 'S';
            QChar writeChar = headers_col[j];
            QString nextState = state;
            bool isHalt = false;

            for (const QString &part : parts) {
                if (part == "L" || part == "R") {
                    direction = part[0];
                } else if (part == "!") {
                    isHalt = true;
                } else if (part.size() == 2 && part[0] == 'q' && part[1].isDigit()) {
                    nextState = part;
                } else if (part.size() == 1) {
                    writeChar = part[0];
                } else {
                    QMessageBox::warning(this, "Стоп", "Не существует подходящего действия");
                }
            }

            if (isHalt) {
                Rule rule {'!', writeChar, nextState};
                kernel->setRule(state, headers_col[j], rule);
            } else {
                Rule rule {direction, writeChar, nextState};
                kernel->setRule(state, headers_col[j], rule);
            }
        }
    }
}