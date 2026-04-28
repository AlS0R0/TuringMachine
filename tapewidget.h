#ifndef TAPEWIDGET_H
#define TAPEWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMap>
#include <QElapsedTimer>

class TuringMachineKernel;

class TapeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double caretLogicalX READ caretLogicalX WRITE setCaretLogicalX)

public:
    explicit TapeWidget(QWidget *parent = nullptr);
    ~TapeWidget();

    void setKernel(TuringMachineKernel *kernel);
    void animateStep();   // запускает плавное перемещение каретки к текущему head_

    double caretLogicalX() const;
    void setCaretLogicalX(double x);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onAnimationFinished();

private:
    TuringMachineKernel *m_kernel = nullptr;
    QPropertyAnimation *m_animation = nullptr;

    int m_cellWidth = 30;
    int m_cellHeight = 30;
    double m_caretLogicalX = 0.0;  // положение каретки в пикселях на ленте (без учёта сдвига)
    int m_scrollOffset = 0;        // смещение ленты в пикселях

    void adjustScroll();            // резко сдвигает ленту, если каретка у края
    double visualCaretX() const;    // координата каретки на виджете
};
#endif