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
    explicit SimulatorWindow(QVector<QChar> vec, QWidget *parent = nullptr);
    ~SimulatorWindow();

private slots:
    void AddCondition_clicked();
    void RemoveCondition_clicked();
    void ChangeAlphabet_clicked();
    void SetLine_clicked();
    void StartMachine_clicked();
    void StepMachine_clicked();
    void ContinueButton_clicked();
    void PauseMachine_clicked();
    void StopMachine_clicked();
    void ResetLine_clicked();
    void loadRulesFromTable();
    void IncSpeed_clicked();
    void DecSpeed_clicked();

private:
    Ui::SimulatorWindow *ui;
    ConditionTable *model;
    TapeWidget *m_tapeWidget;
    TuringMachineKernel *kernel;
    QTableView *tableView;
    QTimer *Timer = nullptr;
    QString initialInput_;


    void setControlsEnabled(bool enabled);
};

#endif // SIMULATORWINDOW_H
