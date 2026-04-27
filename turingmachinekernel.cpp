#include "turingmachinekernel.h"
#include <QDebug>


void TuringMachineKernel::clear() {
    tape_.clear();
    rules_.clear();
    head_ = 0;
    currentState_ = "q0";
}

void TuringMachineKernel::reset(const QString &input) {
    tape_.clear();
    rules_.clear();
    head_ = 0;
    currentState_ = "q0";

    setInputString(input);
}

void TuringMachineKernel::setBlank(QChar symbol) {
    blank_symbol_ = symbol;
}

void TuringMachineKernel::setInputString(const QString &input) {
    tape_.clear();

    for (int i = 0; i < input.size(); ++i)
        tape_[i] = input[i];
}

void TuringMachineKernel::setRule(const QString &state, QChar symbol, const Rule &rule) {
    rules_[state][symbol] = rule;
}

bool TuringMachineKernel::step() {
    QChar current = tape_.contains(head_) ? tape_[head_] : blank_symbol_;

    if (!rules_.contains(currentState_) || !rules_[currentState_].contains(current))
        return false;

    Rule rule = rules_[currentState_][current];

    tape_[head_] = rule.new_symbol;
    if (rule.direction == 'R') head_ += 1;
    else if (rule.direction == 'L') head_ -= 1;
    else qDebug() << "Error";

    currentState_ = rule.nextState;

    return true;
}

QString TuringMachineKernel::getState() const {
    return currentState_;
}

int TuringMachineKernel::getHead() const {
    return head_;
}

QChar TuringMachineKernel::getSymbol(int pos) const {
    return tape_.contains(pos) ? tape_[pos] : blank_symbol_;
}
