#include "frame.h"
#include <QDebug>
#include <QLayout>
#include <QTimer>

Frame::Frame(QWidget *parent)
    : QFrame(parent),
      mIsResizing(false)
{
    setMouseTracking(true);
    mMinWidth = minimumWidth();

}

void Frame::setAlignment(Qt::AlignmentFlag fl)
{
    mAlignmentFlag=fl;
}


void Frame::enterEvent(QEvent *event)
{
    QPoint pos = mapFromGlobal(QCursor::pos());
    if (pos.x() <= 2 || pos.x() >= width() - 2 || pos.y() <= 2 || pos.y() >= height() - 2)
    {
        setCursor(Qt::SizeAllCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void Frame::leaveEvent(QEvent *event)
{
    setCursor(Qt::ArrowCursor);
}


void Frame::mousePressEvent(QMouseEvent *event)
{
    QPoint newPos = (event->pos());
    //qDebug()<<pos;
    if ((newPos.x() <= 4 || newPos.x() >= width() - 4 || newPos.y() <= 4 || newPos.y() >= height() - 4) && event->buttons() == Qt::LeftButton)
    {
        mIsResizing = true;
        mStartPoint = newPos;
    }
}

void Frame::mouseReleaseEvent(QMouseEvent *event)
{
    mIsResizing=false;
}

void Frame::mouseMoveEvent(QMouseEvent *event)
{

    //QTimer m_resizeTimer;
    //QFrame::mouseMoveEvent(event);
    QPoint newPos = (event->pos());
    if (newPos.x() <= 4 || newPos.x() >= width() - 4 || newPos.y() <= 4 || newPos.y() >= height() - 4)
    {
        setCursor(Qt::SizeHorCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
    if (mIsResizing)
    {
        //m_resizeTimer.start(10);

        //setUpdatesEnabled(false);
        int deltaX = mapToGlobal(newPos).x() - mapToGlobal(mStartPoint).x();
        auto curWidth = width();
        if (mAlignmentFlag==Qt::AlignRight)
        {
            auto newWidth = curWidth - deltaX;
            if (newWidth>500 || newWidth<10)
                return;
            setMinimumWidth(newWidth);

            mStartPoint=newPos;

        }
        else
        {
            auto newWidth = curWidth + deltaX;
            if (newWidth>500 || newWidth<10)
                return;

            setMinimumWidth(newWidth);
            mStartPoint=newPos;
        }
//        setUpdatesEnabled(true);
//        update();





    }
}
