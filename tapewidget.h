#ifndef TAPEWIDGET_H
#define TAPEWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMap>
#include "turingmachinekernel.h"

class TuringMachineKernel; // forward declaration

class TapeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double headVisualPos READ headVisualPos WRITE setHeadVisualPos)

public:
    explicit TapeWidget(QWidget *parent = nullptr);
    ~TapeWidget();

    // Привязка к логическому ядру (поставщик данных ленты)
    void setKernel(TuringMachineKernel *kernel);

    // Установка множителя скорости (0.1 – очень медленно, 1.0 – обычно)
    void setSpeed(double factor);

    // Установка символа для пустой ячейки (по умолчанию '^')
    void setBlankSymbol(const QChar &ch);

    // Плавное перемещение каретки к логической позиции
    void moveHeadTo(int logicalPos, bool animate = true);

    // Мгновенное перемещение (без анимации)
    void setHeadPosition(int logicalPos);

    // Загрузка входной строки на ленту (сбрасывает позицию головки)
    void setTapeContent(const QString &input);

    // Сброс – возврат к исходному состоянию (пустая лента, головка в 0)
    void reset();

    // Текущая логическая позиция головки
    int currentHeadPosition() const;

    // Символ под головкой
    QChar currentSymbol() const;

signals:
    // Излучается, когда анимация одного шага полностью завершена.
    // Полезно для пошагового или автоматического выполнения программы.
    void stepFinished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onAnimationFinished();

private:
    void updateTapeOffsetIfNeeded();
    void stopAnimation();

    // Свойства для анимации
    double headVisualPos() const;
    void setHeadVisualPos(double pos);

    // ===== Данные =====
    TuringMachineKernel *m_kernel = nullptr;

    int m_headLogicalPos = 0;       // логическая позиция (целое)
    double m_headVisualPos = 0.0;   // визуальная позиция (дробная, для анимации)
    int m_tapeOffset = 0;           // смещение левого края видимой области в логических индексах

    QChar m_blankSymbol = '^';      // обозначение пустого символа

    double m_speed = 1.0;           // множитель скорости
    int m_baseAnimationDuration = 300; // базовая длительность перемещения на 1 ячейку (мс)

    QPropertyAnimation *m_animation = nullptr;
    bool m_animating = false;

    // Визуальные параметры
    int m_cellSize = 60;            // ширина/высота ячейки в пикселях
    int m_visibleCells = 15;        // максимальное количество отображаемых ячеек
};

#endif // TAPEWIDGET_H
