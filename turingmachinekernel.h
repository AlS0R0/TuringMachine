#ifndef TURINGMACHINEKERNEL_H
#define TURINGMACHINEKERNEL_H

#include <QObject>
#include <QMap>
#include <QPair>
#include <QSet>
#include <QVector>
#include <QString>

struct Transition {
    QString newState;
    QChar newSymbol;
    char direction;
};

class TuringMachineKernel : public QObject
{
    Q_OBJECT

public:
    explicit TuringMachineKernel(QObject *parent = nullptr);


    void setAlphabet(const QSet<QChar> &tapeAlphabet);
    QSet<QChar> getAlphabet() const { return m_tapeAlphabet; }
    QChar getBlankSymbol() const { return m_blankSymbol; }


    void setStates(const QVector<QString> &states);
    QVector<QString> getStates() const { return m_states; }
    void addState(const QString &state);
    void removeState(const QString &state);
    QString getInitialState() const { return m_initialState; }
    QString getHaltState() const { return m_haltState; }
    bool isHaltState(const QString &state) const { return state == m_haltState; }


    void setRule(const QString &state, QChar symbol, const Transition &trans);
    Transition getRule(const QString &state, QChar symbol) const;
    bool hasRule(const QString &state, QChar symbol) const;
    void clearRules();


    void setInputString(const QString &input);
    void resetToInitial();
    QChar getSymbolAt(int position) const;
    void setSymbolAt(int position, QChar symbol);
    int getHeadPosition() const { return m_headPos; }
    void setHeadPosition(int pos);
    QString getCurrentState() const { return m_currentState; }


    bool canStep() const;
    void step();
    bool isHalted() const { return m_halted; }
    bool hasAnyHaltTransition() const;

signals:
    void machineStopped();
    void ruleExecuted(const Transition &trans, int oldHeadPos);
    void headMoved(int newPos);
    void tapeChanged(int pos, QChar newSymbol);

private:
    QSet<QChar> m_tapeAlphabet;
    QChar m_blankSymbol = QChar('^');

    QVector<QString> m_states;
    QString m_initialState = "q0";
    QString m_haltState = "^";

    QMap<QPair<QString, QChar>, Transition> m_rules;

    QMap<int, QChar> m_tape;
    int m_headPos = 0;
    QString m_currentState;
    bool m_halted = false;

    void applyTransition(const Transition &trans);
};

#endif // TURINGMACHINEKERNEL_H