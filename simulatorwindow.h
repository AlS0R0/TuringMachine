#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QDialog>
#include "conditiontable.h"
#include "tapewidget.h"
#include <QTableView>
#include <QTimer>

namespace Ui {
class SimulatorWindow;
}

class SimulatorWindow : public QDialog
{
    Q_OBJECT

public:
    // explicit SimulatorWindow(QWidget *parent = nullptr);
    explicit SimulatorWindow(QString str1, QString str2, QWidget *parent = nullptr);
    ~SimulatorWindow();

private slots:
    void AddCondition_clicked();
    void RemoveCondition_clicked();
    void ChangeAlphabet_clicked();
    void SetLine_clicked();
    void StartMachine_clicked();
    void StepMachine_clicked();
    void PauseMachine_clicked();
    void StopMachine_clicked();
    void ResetLine_clicked();
    void loadRulesFromTable();

private:
    Ui::SimulatorWindow *ui;
    ConditionTable *model;
    TapeWidget *m_tapeWidget;
    TuringMachineKernel *m_kernel;
    QTableView *tableView;
    QTimer *m_stepTimer = nullptr;          // <-- добавлено
    QString m_initialInput;                 // <-- исходная строка для сброса

    // вспомогательный метод блокировки интерфейса во время работы
    void setControlsEnabled(bool enabled);
};

#endif // SIMULATORWINDOW_H
