#ifndef TAPEWIDGET_H
#define TAPEWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMap>

class TuringMachineKernel;

class TapeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TapeWidget(QWidget *parent = nullptr);
    ~TapeWidget();
};

#endif // TAPEWIDGET_H