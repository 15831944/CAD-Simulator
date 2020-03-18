#ifndef QXTSPANSLIDER_P_H
#define QXTSPANSLIDER_P_H


#include <QStyle>
#include <QObject>
#include "qxtspanslider.h"

QT_FORWARD_DECLARE_CLASS(QStylePainter)
QT_FORWARD_DECLARE_CLASS(QStyleOptionSlider)

class QxtSpanSliderPrivate : public QObject {
    Q_OBJECT
public:
    QxtSpanSliderPrivate();
    void initStyleOption(QStyleOptionSlider* option, QxtSpanSlider::SpanHandle handle = QxtSpanSlider::UpperHandle) const;
    int pick(const QPoint& pt) const
    {
        return q_ptr->orientation() == Qt::Horizontal ? pt.x() : pt.y();
    }
    int pixelPosToRangeValue(int pos) const;
    void handleMousePress(const QPoint& pos, QStyle::SubControl& control, int value, QxtSpanSlider::SpanHandle handle);
    void drawHandle(QStylePainter* painter, QxtSpanSlider::SpanHandle handle) const;
    void drawLowHandle(QStylePainter* painter, QxtSpanSlider::SpanHandle handle) const;
    void drawHeiHandle(QStylePainter* painter, QxtSpanSlider::SpanHandle handle) const;
    void drawNowHandle(QStylePainter* painter, QxtSpanSlider::SpanHandle handle) const;
    void setupPainter(QPainter* painter, Qt::Orientation orientation, qreal x1, qreal y1, qreal x2, qreal y2) const;
    void drawSpan(QStylePainter* painter, const QRect& rect) const;
    void triggerAction(QAbstractSlider::SliderAction action, bool main);
    void swapControls();

    int lower;
    int upper;
    int nowValue;

    int lowerPos;
    int upperPos;
    int nowPos;

    int offset;
    int position;
    QxtSpanSlider::SpanHandle lastPressed;
    QxtSpanSlider::SpanHandle mainControl;
    QxtSpanSlider::SpanHandle nowControl;
    QStyle::SubControl lowerPressed;
    QStyle::SubControl upperPressed;
    QStyle::SubControl nowPressed;
    QxtSpanSlider::HandleMovementMode movement;
    bool firstMovement;
    bool blockTracking;


public Q_SLOTS:
    void updateRange(int min, int max);
    void movePressedHandle();

private:
    QxtSpanSlider* q_ptr;
    friend class QxtSpanSlider;


};

#endif // QXTSPANSLIDER_P_H
