#ifndef LIPFRAME_H
#define LIPFRAME_H

#include <QFrame>
#include <QMouseEvent>
class LIPFrame : public QFrame
{
public:
    LIPFrame(QWidget *parent = nullptr);
    void setAlignment(Qt::AlignmentFlag fl);
    // QWidget interface
protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    bool mIsResizing;
    Qt::AlignmentFlag mAlignmentFlag;
    QPoint mStartPoint;
};

#endif // LIPFRAME_H
