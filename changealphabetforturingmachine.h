#ifndef CHANGEALPHABETFORTURINGMACHINE_H
#define CHANGEALPHABETFORTURINGMACHINE_H

#include <QDialog>

namespace Ui {
class ChangeAlphabetForTuringMachine;
}

class ChangeAlphabetForTuringMachine : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeAlphabetForTuringMachine(QWidget *parent = nullptr, QVector<QChar>* vec = nullptr);
    ~ChangeAlphabetForTuringMachine();

private slots:

    void pushButton_clicked();

private:
    Ui::ChangeAlphabetForTuringMachine *ui;
    QVector<QChar>* new_alphabet;
};

#endif // CHANGEALPHABETFORTURINGMACHINE_H
