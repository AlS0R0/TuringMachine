#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"
#include "conditiontable.h"
#include "tapewidget.h"
#include "turingmachinekernel.h"
#include <QTableView>
#include <QMessageBox>

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
    //ui->verticalLayout->insertWidget(0, m_tapeWidget); // или заменить существующий QGraphicsView

    // Вставляем ленту в gridLayout_3 на новую строку 0 (сверху)
    ui->verticalLayout->addWidget(m_tapeWidget);

    connect(ui->AddCondition, &QPushButton::clicked, this, &SimulatorWindow::AddCondition_clicked);
    connect(ui->RemoveCondition, &QPushButton::clicked, this, &SimulatorWindow::RemoveCondition_clicked);
    connect(ui->ChangeAlphabet, &QPushButton::clicked, this, &SimulatorWindow::ChangeAlphabet_clicked);
    connect(ui->SetLine, &QPushButton::clicked, this, &SimulatorWindow::SetLine_clicked);
    connect(ui->StartMachine, &QPushButton::clicked, this, &SimulatorWindow::StartMachine_clicked);
    connect(ui->StepMachine, &QPushButton::clicked, this, &SimulatorWindow::StepMachine_clicked);
    connect(ui->PauseMachine, &QPushButton::clicked, this, &SimulatorWindow::PauseMachine_clicked);
    connect(ui->StopMachine, &QPushButton::clicked, this, &SimulatorWindow::StopMachine_clicked);
    connect(ui->ResetLine, &QPushButton::clicked, this, &SimulatorWindow::ResetLine_clicked);

    this->model = new ConditionTable(1, str1, str2, this);

    this->tableView = new QTableView();
    tableView->setModel(this->model);

    ui->gridLayout_5->addWidget(tableView);

    m_stepTimer = new QTimer(this);
    m_stepTimer->setInterval(500);  // 500 мс между шагами
    connect(m_stepTimer, &QTimer::timeout, this, [this]() {
        // Автоматический шаг при срабатывании таймера
        if (m_kernel->canStep()) {
            m_kernel->step();
            m_tapeWidget->moveHeadTo(m_kernel->getHeadPosition(), true);  // с анимацией
        } else {
            m_stepTimer->stop();  // остановка, если нет правил или машина остановилась
            setControlsEnabled(true);
        }
    });

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
    // Кнопки скорости всегда активны
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

void SimulatorWindow::SetLine_clicked()
{
    QString input = ui->LineTape->text();

    m_kernel->setInputString(input);
    m_initialInput = input;

    m_tapeWidget->setHeadPosition(m_kernel->getHeadPosition());
}

void SimulatorWindow::StartMachine_clicked() {
    loadRulesFromTable();



    if (!m_kernel->hasAnyHaltTransition()) {
        QMessageBox::warning(this, "Ошибка", "Нет ни одного правила, ведущего к остановке! "
                             "Добавьте хотя бы одно правило с переходом в состояние останова.");
        return;
    }

    setControlsEnabled(false);
    m_tapeWidget->setHeadPosition(m_kernel->getHeadPosition());
    m_tapeWidget->update();

    if (!m_kernel->hasAnyHaltTransition()) {
        QMessageBox::warning(this, "Ошибка",
                             "Нет ни одного правила, ведущего к остановке!");
        return;
    }

    // Проверка: есть ли правило для текущего состояния/символа
    if (!m_kernel->canStep()) {
        QMessageBox::information(this, "Информация",
                                 "Нет правила для начального состояния и первого символа.");
        return;
    }

    setControlsEnabled(false);
    m_tapeWidget->setHeadPosition(m_kernel->getHeadPosition());

    // 🔑 ЗАПУСК ТАЙМЕРА — машина начнёт работать
    m_stepTimer->start();
}

void SimulatorWindow::StepMachine_clicked() {
    if (m_stepTimer->isActive())
        return;

    if (m_kernel->canStep()) {
        m_kernel->step();
        m_tapeWidget->moveHeadTo(m_kernel->getHeadPosition());
    } else {
        QMessageBox::information(this, "Шаг", "Невозможно выполнить шаг (нет правила или машина остановлена).");
    }
}

void SimulatorWindow::PauseMachine_clicked() {
    m_stepTimer->stop();
}

void SimulatorWindow::StopMachine_clicked() {
    m_stepTimer->stop();
    setControlsEnabled(true);
}

void SimulatorWindow::ResetLine_clicked() {
    m_stepTimer->stop();

    // Восстанавливаем исходную строку и помещаем головку на начало
    m_kernel->setInputString(m_initialInput);
    m_tapeWidget->setHeadPosition(m_kernel->getHeadPosition());
    m_tapeWidget->update();

    setControlsEnabled(true);
}

void SimulatorWindow::loadRulesFromTable()
{
    if (!model || !m_kernel) return;

    // 1. Синхронизируем алфавит ядра с заголовками таблицы
    QVector<QChar> headers = model->getColumnHeaders();
    QSet<QChar> alphabet(headers.begin(), headers.end());
    m_kernel->setAlphabet(alphabet);

    // 2. Очищаем старые правила
    m_kernel->clearRules();

    // 🔑 Важно: передаём QModelIndex(), т.к. в вашем ConditionTable нет аргумента по умолчанию
    int rows = model->rowCount(QModelIndex());
    int cols = model->columnCount(QModelIndex());

    for (int r = 0; r < rows; ++r) {
        QString currentState = QString("q%1").arg(r);
        m_kernel->addState(currentState);

        for (int c = 0; c < cols; ++c) {
            QModelIndex idx = model->index(r, c);
            QVariant val = model->data(idx, Qt::DisplayRole);

            if (val.isValid() && !val.toString().trimmed().isEmpty()) {
                QString cellText = val.toString().trimmed();
                QStringList parts = cellText.split(',', Qt::SkipEmptyParts);

                // 🔑 Поддержка 1, 2 или 3 значений
                if (parts.size() >= 1 && parts.size() <= 3) {

                    // === Парсинг нового состояния (обязательно) ===
                    QString newState = parts[0].trimmed();
                    // 🔑 Знак "!" = состояние останова
                    if (newState == "!") {
                        newState = m_kernel->getHaltState();
                    }

                    // === Парсинг нового символа (опционально) ===
                    QChar newSymbol;
                    bool keepCurrentSymbol = true;  // флаг: не менять символ на ленте
                    if (parts.size() >= 2) {
                        QString symStr = parts[1].trimmed();
                        if (!symStr.isEmpty()) {
                            newSymbol = symStr[0];
                            keepCurrentSymbol = false;
                        }
                    }

                    // === Парсинг направления (опционально) ===
                    char direction = 'S';  // по умолчанию — стоять на месте
                    if (parts.size() >= 3) {
                        QString dirStr = parts[2].trimmed();
                        if (!dirStr.isEmpty()) {
                            direction = dirStr[0].toUpper().toLatin1(); // L, R или S
                        }
                    }

                    // === Сборка перехода ===
                    Transition trans;
                    trans.newState = newState;

                    // Если символ не указан — оставляем текущий символ на ленте без изменений
                    if (keepCurrentSymbol) {
                        QChar currentSymbol = (c < headers.size()) ? headers[c] : QChar('^');
                        trans.newSymbol = currentSymbol;
                    } else {
                        trans.newSymbol = newSymbol;
                    }
                    trans.direction = direction;

                    // Регистрируем новое состояние в ядре
                    m_kernel->addState(newState);

                    // Текущий символ для ключа правила
                    QChar currentSymbol = (c < headers.size()) ? headers[c] : QChar('^');
                    m_kernel->setRule(currentState, currentSymbol, trans);

                } else {
                    qWarning() << "Ошибка формата в ячейке (" << r << "," << c << "):"
                               << "ожидается 1-3 значения через запятую, получено:" << cellText;
                }
            }
        }
    }
}