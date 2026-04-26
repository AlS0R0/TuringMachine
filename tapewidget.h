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
    // Убрали NOTIFY – для анимации он не нужен
    Q_PROPERTY(double headVisualPos READ headVisualPos WRITE setHeadVisualPos)

public:
    explicit TapeWidget(QWidget *parent = nullptr);
    ~TapeWidget();

    // Привязка к ядру машины Тьюринга (обязательно)
    void setKernel(TuringMachineKernel *kernel);

    // Управление скоростью перемещения (1.0 – нормальная)
    void setSpeed(double factor);
    double speed() const { return m_speed; }

    // Символ для отображения пустой ячейки (по умолчанию '^')
    void setBlankSymbol(const QChar &ch);
    QChar blankSymbol() const { return m_blankSymbol; }

    // Задать количество видимых ячеек (динамически)
    void setVisibleCells(int count);
    int visibleCells() const { return m_visibleCells; }

    // Задать размер одной ячейки в пикселях
    void setCellSize(int size);
    int cellSize() const { return m_cellSize; }

    // Переместить головку в логическую позицию (с анимацией или без)
    void moveHeadTo(int logicalPos, bool animate = true);
    // Мгновенно установить позицию (без анимации)
    void setHeadPosition(int logicalPos);

    // Загрузить строку на ленту и установить головку в начало
    void setTapeContent(const QString &input);

    // Сбросить виджет (пустая лента, головка в 0)
    void reset();

    // Текущие данные
    int currentHeadPosition() const;
    QChar currentSymbol() const;

signals:
    // Излучается по завершении анимации одного шага
    void stepFinished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onAnimationFinished();

private:
    void stopAnimation();
    void updateTapeOffsetIfNeeded();

    double headVisualPos() const;
    void setHeadVisualPos(double pos);

    // Ядро
    TuringMachineKernel *m_kernel = nullptr;

    // Позиции
    int m_headLogicalPos = 0;
    double m_headVisualPos = 0.0;
    int m_tapeOffset = 0;             // смещение левой видимой ячейки (логический индекс)

    // Внешний вид
    QChar m_blankSymbol = '^';
    int m_cellSize = 30;              // ширина и высота ячейки
    int m_visibleCells = 15;          // увеличено по умолчанию

    // Анимация
    double m_speed = 1.0;
    int m_baseAnimationDuration = 300; // базовое время на 1 ячейку (мс)
    QPropertyAnimation *m_animation = nullptr;
    bool m_animating = false;
};

#endif // TAPEWIDGET_H