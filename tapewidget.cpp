#include "tapewidget.h"
#include "turingmachinekernel.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QtMath>

TapeWidget::TapeWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(400, 70);
    m_animation = new QPropertyAnimation(this, "caretLogicalX", this);
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::InOutCubic);
    connect(m_animation, &QPropertyAnimation::finished,
            this, &TapeWidget::onAnimationFinished);
}

TapeWidget::~TapeWidget() {}

void TapeWidget::setKernel(TuringMachineKernel *kernel)
{
    m_kernel = kernel;
    if (m_kernel) {
        m_caretLogicalX = m_kernel->getHead() * m_cellWidth;
        m_scrollOffset = 0;                // сбрасываем смещение
        adjustScroll();
        update();
    }
}

double TapeWidget::caretLogicalX() const
{
    return m_caretLogicalX;
}

void TapeWidget::setCaretLogicalX(double x)
{
    m_caretLogicalX = x;
    update();
}

void TapeWidget::animateStep()
{
    if (!m_kernel) return;
    qDebug() << "in animateStep";
    int targetIndex = m_kernel->getHead();
    qDebug() << "targetIndex:  " << targetIndex;
    double targetX = targetIndex * m_cellWidth;
    qDebug() << "targetX: " << targetX;
    if (qFuzzyCompare(targetX, m_caretLogicalX)) {
        adjustScroll();
        update();
        // qDebug() << "Остаемся на месте или двигаемся";
        return;
    }
    qDebug() << "В анимации";
    m_animation->stop();
    m_animation->setStartValue(m_caretLogicalX);
    m_animation->setEndValue(targetX);
    m_animation->start();
}

void TapeWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor("#f0f0f0")); // светло-серый фон ленты

    if (!m_kernel) return;

    int leftCell = qFloor((double)m_scrollOffset / m_cellWidth) - 1;
    int rightCell = qCeil((double)(m_scrollOffset + width()) / m_cellWidth) + 1;

    for (int i = leftCell; i <= rightCell; ++i) {
        int x = i * m_cellWidth - m_scrollOffset;
        QRect cellRect(x, 5, m_cellWidth, m_cellHeight);

        // Белый фон ячейки
        painter.fillRect(cellRect, Qt::white);
        painter.setPen(QPen(Qt::gray, 1));
        painter.drawRect(cellRect);

        QChar ch = m_kernel->getSymbol(i);
        QString symbol = (ch == '^') ? QString::fromUtf8("\u039B") : QString(ch);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Consolas", 16));
        painter.drawText(cellRect, Qt::AlignCenter, symbol);
    }

    // Подсветка текущей ячейки (синяя рамка)
    int headIndex = m_kernel->getHead();
    int headCellX = headIndex * m_cellWidth - m_scrollOffset;
    QRect headRect(headCellX, 5, m_cellWidth, m_cellHeight);
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawRect(headRect);

    // Красный треугольник-каретка под ячейкой
    double caretVisX = visualCaretX();
    int caretX = qRound(caretVisX) + m_cellWidth / 2;      // центр ячейки
    int caretY = 5 + m_cellHeight + 4;                     // чуть ниже ячейки
    QPolygon triangle;
    triangle << QPoint(caretX, caretY)
             << QPoint(caretX - 6, caretY + 10)
             << QPoint(caretX + 6, caretY + 10);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::red);
    painter.drawPolygon(triangle);
}

void TapeWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    adjustScroll();
    update();
}

void TapeWidget::onAnimationFinished()
{
    m_caretLogicalX = m_kernel->getHead() * m_cellWidth;
    adjustScroll();
    update();
}

void TapeWidget::adjustScroll()
{
    if (!m_kernel) return;
    double caretVisX = visualCaretX();
    double marginLeft = width() * 0.25;
    double marginRight = width() * 0.75;

    if (caretVisX < marginLeft) {
        double delta = marginLeft - caretVisX;
        m_scrollOffset = qMax(0, m_scrollOffset - (int)delta);
    } else if (caretVisX > marginRight) {
        double delta = caretVisX - marginRight;
        m_scrollOffset += (int)delta;
    }
    m_caretLogicalX = m_kernel->getHead() * m_cellWidth;
}

double TapeWidget::visualCaretX() const
{
    return m_caretLogicalX - m_scrollOffset;
}