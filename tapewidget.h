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
    void animateStep();

    int scrollOffset() const;
    void setScrollOffset(int offset);

    double caretLogicalX() const;
    void setCaretLogicalX(double x);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onAnimationFinished();

signals:
    void scrollOffsetChanged(int offset);

private:
    TuringMachineKernel *m_kernel = nullptr;
    QPropertyAnimation *m_animation = nullptr;

    int m_cellWidth = 30;
    int m_cellHeight = 30;
    double m_caretLogicalX = 0.0;
    int m_scrollOffset = 0;

    void adjustScroll();
    double visualCaretX() const;
};
#endif