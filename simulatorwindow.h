#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QDialog>
#include "conditiontable.h"
#include <QTableView>

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

private:
    Ui::SimulatorWindow *ui;
    ConditionTable *model;
    QTableView *tableView;
};

#endif // SIMULATORWINDOW_H
