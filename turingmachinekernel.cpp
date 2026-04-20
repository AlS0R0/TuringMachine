#include "turingmachinekernel.h"
#include <QDebug>

TuringMachineKernel::TuringMachineKernel(QObject *parent) : QObject(parent)
{
    m_tapeAlphabet.insert(m_blankSymbol);
    m_states = {m_initialState, m_haltState};
    m_currentState = m_initialState;
}

void TuringMachineKernel::setAlphabet(const QSet<QChar> &tapeAlphabet)
{
    m_tapeAlphabet = tapeAlphabet;
    m_tapeAlphabet.insert(m_blankSymbol);
}

void TuringMachineKernel::setStates(const QVector<QString> &states)
{
    m_states = states;

    if (!m_states.contains(m_initialState)) {
        m_initialState = m_states.first();
    }

    if (!m_states.contains(m_haltState)) {
        m_states.append(m_haltState);
    }
}

void TuringMachineKernel::addState(const QString &state)
{
    if (!m_states.contains(state)) {
        m_states.append(state);
    }
}

void TuringMachineKernel::removeState(const QString &state)
{
    if (state == m_haltState || state == m_initialState) return;
    m_states.removeAll(state);
    QMutableMapIterator<QPair<QString, QChar>, Transition> it(m_rules);
    while (it.hasNext()) {
        it.next();
        if (it.key().first == state || it.value().newState == state) {
            it.remove();
        }
    }
}

void TuringMachineKernel::setRule(const QString &state, QChar symbol, const Transition &trans)
{
    if (!m_tapeAlphabet.contains(symbol) && symbol != m_blankSymbol) return;

    if (!m_tapeAlphabet.contains(trans.newSymbol) && trans.newSymbol != m_blankSymbol) return;

    if (!m_states.contains(trans.newState) && trans.newState != m_haltState) return;

    m_rules[{state, symbol}] = trans;
}

Transition TuringMachineKernel::getRule(const QString &state, QChar symbol) const
{
    return m_rules.value({state, symbol});
}

bool TuringMachineKernel::hasRule(const QString &state, QChar symbol) const
{
    return m_rules.contains({state, symbol});
}

void TuringMachineKernel::clearRules()
{
    m_rules.clear();
}

void TuringMachineKernel::setInputString(const QString &input)
{
    m_tape.clear();
    m_headPos = 0;

    for (int i = 0; i < input.length(); ++i) {

        QChar ch = input[i];

        if (m_tapeAlphabet.contains(ch) || ch == m_blankSymbol) {
            m_tape[i] = ch;
        } else {
            qWarning() << "Invalid symbol in input string:" << ch;
        }
    }
    m_currentState = m_initialState;
    m_halted = false;
}

void TuringMachineKernel::resetToInitial()
{
    m_tape.clear();
    m_headPos = 0;
    m_currentState = m_initialState;
    m_halted = false;
}

QChar TuringMachineKernel::getSymbolAt(int position) const
{
    return m_tape.value(position, m_blankSymbol);
}

void TuringMachineKernel::setSymbolAt(int position, QChar symbol)
{
    if (symbol == m_blankSymbol) {
        m_tape.remove(position);
    } else {
        m_tape[position] = symbol;
    }
    emit tapeChanged(position, symbol);
}

void TuringMachineKernel::setHeadPosition(int pos)
{
    m_headPos = pos;
    emit headMoved(pos);
}

bool TuringMachineKernel::canStep() const
{
    return !m_halted && hasRule(m_currentState, getSymbolAt(m_headPos));
}

void TuringMachineKernel::step()
{
    if (!canStep()) {
        m_halted = true;
        emit machineStopped();
        return;
    }

    QChar currentSymbol = getSymbolAt(m_headPos);
    Transition trans = getRule(m_currentState, currentSymbol);
    int oldPos = m_headPos;

    if (trans.newSymbol != currentSymbol) {
        setSymbolAt(m_headPos, trans.newSymbol);
    }

    switch (trans.direction) {
        case 'L': m_headPos--; break;
        case 'R': m_headPos++; break;
        default: break;
    }

    m_currentState = trans.newState;

    emit ruleExecuted(trans, oldPos);
    emit headMoved(m_headPos);

    if (isHaltState(m_currentState)) {
        m_halted = true;
        emit machineStopped();
    }
}

bool TuringMachineKernel::hasAnyHaltTransition() const
{
    for (auto it = m_rules.begin(); it != m_rules.end(); ++it) {
        if (it.value().newState == m_haltState) return true;
    }
    return false;
}