#include "tapewidget.h"
#include "turingmachinekernel.h"
#include <QPainter>
#include <QFontMetrics>
#include <QPaintEvent>
#include <QtMath>
#include <QDebug>

TapeWidget::TapeWidget(QWidget *parent)
    : QWidget(parent)
    , m_animation(new QPropertyAnimation(this, "headVisualPos"))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMinimumHeight(m_cellSize + 30); // с запасом для треугольника

    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_animation, &QPropertyAnimation::finished,
            this, &TapeWidget::onAnimationFinished);
}

TapeWidget::~TapeWidget() {}

// ==============================
//        Публичные методы
// ==============================

void TapeWidget::setKernel(TuringMachineKernel *kernel)
{
    if (m_kernel) {
        disconnect(m_kernel, nullptr, this, nullptr);
    }
    m_kernel = kernel;
    if (m_kernel) {
        connect(m_kernel, &TuringMachineKernel::headMoved,
                this, &TapeWidget::setHeadPosition);
        connect(m_kernel, &TuringMachineKernel::tapeChanged,
                this, [this]() { update(); });
    }
    update();
}

void TapeWidget::setSpeed(double factor)
{
    m_speed = qMax(0.1, factor);
}

void TapeWidget::setBlankSymbol(const QChar &ch)
{
    m_blankSymbol = ch;
    update();
}

void TapeWidget::setVisibleCells(int count)
{
    if (count < 5) count = 5;
    m_visibleCells = count;
    updateTapeOffsetIfNeeded();
    update();
}

void TapeWidget::setCellSize(int size)
{
    if (size < 30) size = 30;
    m_cellSize = size;
    setMinimumHeight(size + 30);
    updateTapeOffsetIfNeeded();
    update();
}

void TapeWidget::moveHeadTo(int logicalPos, bool animate)
{
    if (!m_kernel) return;

    if (!animate) {
        setHeadPosition(logicalPos);
        return;
    }

    stopAnimation();

    double startPos = m_headVisualPos;
    double endPos = logicalPos;
    double distance = qAbs(endPos - startPos);

    if (distance < 0.001) {
        onAnimationFinished();
        return;
    }

    int duration = qMax(50, qRound(m_baseAnimationDuration * distance / m_speed));
    m_animation->setDuration(duration);
    m_animation->setStartValue(startPos);
    m_animation->setEndValue(endPos);

    m_animating = true;
    m_animation->start();
}

void TapeWidget::setHeadPosition(int logicalPos)
{
    stopAnimation();
    m_headLogicalPos = logicalPos;
    m_headVisualPos = logicalPos;
    updateTapeOffsetIfNeeded();
    update();
}

void TapeWidget::setTapeContent(const QString &input)
{
    if (m_kernel) {
        m_kernel->setInputString(input);
        setHeadPosition(m_kernel->getHeadPosition());
    }
}

void TapeWidget::reset()
{
    stopAnimation();
    if (m_kernel) {
        m_kernel->resetToInitial();
        setHeadPosition(m_kernel->getHeadPosition());
    }
}

int TapeWidget::currentHeadPosition() const
{
    return m_headLogicalPos;
}

QChar TapeWidget::currentSymbol() const
{
    if (m_kernel)
        return m_kernel->getSymbolAt(m_headLogicalPos);
    return m_blankSymbol;
}

// ==============================
//          Отрисовка
// ==============================

void TapeWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int w = width();
    int h = height();
    painter.fillRect(0, 0, w, h, QColor(240, 240, 240));

    if (!m_kernel || m_visibleCells <= 0)
        return;

    int cellW = m_cellSize;
    int totalWidth = cellW * m_visibleCells;
    int startX = (w - totalWidth) / 2;
    int startY = (h - cellW - 30) / 2;   // 30 – отступ для треугольника

    int firstLogical = m_tapeOffset;
    int lastLogical  = firstLogical + m_visibleCells - 1;

    QFont font("Arial", 14);
    painter.setFont(font);
    QFontMetrics fm(font);

    // 1. Рисуем ячейки ленты (без активной подсветки)
    for (int logIdx = firstLogical; logIdx <= lastLogical; ++logIdx) {
        int visIdx = logIdx - firstLogical;
        int x = startX + visIdx * cellW;

        painter.setPen(QPen(QColor(180, 180, 180), 1));
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(x, startY, cellW, cellW);

        QChar symbol = m_kernel->getSymbolAt(logIdx);
        if (symbol.isNull())
            symbol = m_blankSymbol;

        painter.setPen(Qt::black);
        painter.drawText(QRect(x, startY, cellW, cellW),
                         Qt::AlignCenter, QString(symbol));
    }

    // 2. Подсветка активной ячейки – полупрозрачный прямоугольник
    double headVis = m_headVisualPos;
    double headX = startX + (headVis - m_tapeOffset) * cellW;
    painter.setBrush(QColor(220, 20, 60, 100));   // красный, прозрачность 100/255
    painter.setPen(Qt::NoPen);
    painter.drawRect(QRectF(headX, startY, cellW, cellW));

    // 3. Треугольник-указатель под лентой (перевёрнут остриём вверх)
    double centerX = headX + cellW / 2.0;
    double tipY      = startY + cellW + 4;      // кончик сразу под ячейкой
    double baseY     = tipY + 14;               // основание ниже
    double halfWidth = 10;

    QPointF points[3] = {
        QPointF(centerX, tipY),                     // вершина (вверх)
        QPointF(centerX - halfWidth, baseY),        // левый нижний
        QPointF(centerX + halfWidth, baseY)         // правый нижний
    };

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(220, 20, 60, 200));     // красный, почти непрозрачный
    painter.drawPolygon(points, 3);
}

void TapeWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // Автоматически подбираем количество видимых ячеек под ширину виджета
    // Если хотите фиксированное число – закомментируйте эту строку
    m_visibleCells = qMax(5, width() / m_cellSize);
    updateTapeOffsetIfNeeded();
    update();
}

// ==============================
//   Вспомогательные методы
// ==============================

void TapeWidget::onAnimationFinished()
{
    m_animating = false;
    m_headLogicalPos = qRound(m_headVisualPos);
    m_headVisualPos = m_headLogicalPos;

    updateTapeOffsetIfNeeded();
    update();

    emit stepFinished();
}

void TapeWidget::updateTapeOffsetIfNeeded()
{
    if (m_visibleCells <= 0) return;

    int margin = qMax(2, m_visibleCells / 4);   // "треть или четверть"
    int headVisIdx = m_headLogicalPos - m_tapeOffset;

    if (headVisIdx < margin) {
        m_tapeOffset = m_headLogicalPos - margin;
    } else if (headVisIdx > m_visibleCells - margin - 1) {
        m_tapeOffset = m_headLogicalPos - (m_visibleCells - margin - 1);
    }

    m_tapeOffset = qMax(m_tapeOffset, m_headLogicalPos - m_visibleCells + 1);
    m_tapeOffset = qMin(m_tapeOffset, m_headLogicalPos);
}

void TapeWidget::stopAnimation()
{
    if (m_animating) {
        m_animation->stop();
        m_animating = false;
    }
}

double TapeWidget::headVisualPos() const { return m_headVisualPos; }

void TapeWidget::setHeadVisualPos(double pos) {
    if (!qFuzzyCompare(m_headVisualPos, pos)) {
        m_headVisualPos = pos;
        update();
    }
}