#ifndef TURINGMACHINEKERNEL_H
#define TURINGMACHINEKERNEL_H

#include <QMap>
#include <QString>

struct Rule {
    QChar direction;
    QChar new_symbol;
    QString nextState;
};

class TuringMachineKernel {
public:
    TuringMachineKernel() = default;
    void clear();
    void reset(const QString &input);
    void setInputString(const QString &input);
    void setRule(const QString &state, QChar symbol, const Rule &rule);
    void setBlank(QChar b);

    bool step();

    QString getState() const;
    int getHead() const;
    QChar getSymbol(int pos) const;

private:
    QMap<int,QChar> tape_;
    QMap<QString, QMap<QChar, Rule>> rules_;

    int head_ = 0;
    QString currentState_ = "q0";
    QChar blank_symbol_ = '^';
};

#endif