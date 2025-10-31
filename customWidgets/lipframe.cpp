#include "lipframe.h"
#include <QDebug>
#include <QLayout>

LIPFrame::LIPFrame(QWidget *parent)
    : QFrame(parent),
      mIsResizing(false)
{
    setMouseTracking(true);

}

void LIPFrame::setAlignment(Qt::AlignmentFlag fl)
{
    mAlignmentFlag=fl;
}


void LIPFrame::enterEvent(QEvent *event)
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

void LIPFrame::leaveEvent(QEvent *event)
{
    setCursor(Qt::ArrowCursor);
}


void LIPFrame::mousePressEvent(QMouseEvent *event)
{
    QPoint newPos = (event->pos());
    //qDebug()<<pos;
    if ((newPos.x() <= 4 || newPos.x() >= width() - 4 || newPos.y() <= 4 || newPos.y() >= height() - 4) && event->buttons() == Qt::LeftButton)
    {
        mIsResizing = true;
        mStartPoint = newPos;
    }
}

void LIPFrame::mouseReleaseEvent(QMouseEvent *event)
{
    mIsResizing=false;
}

void LIPFrame::mouseMoveEvent(QMouseEvent *event)
{
    QFrame::mouseMoveEvent(event);
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

        int deltaX = mapToGlobal(newPos).x() - mapToGlobal(mStartPoint).x();
        qDebug()<< "currentPos" + QString::number(mapToGlobal(newPos).x());
        qDebug()<< "currentPos" + QString::number(mapToGlobal(mStartPoint).x());
        //int deltaY = newPos.y() - mapToParent(mStartPoint).y();
        QRect currentGeometry = geometry();
        QRect newGeometry = currentGeometry.adjusted(deltaX, 0, 0, 0);
        qDebug()<<"Delta: " + QString::number(deltaX);
        if (mAlignmentFlag==Qt::AlignRight)
        {
            if (minimumWidth()-deltaX>500)
                return;
            setMinimumWidth(minimumWidth()-deltaX);

        }
        else
        {
            if (minimumWidth()+deltaX>500)
                return;
            setMinimumWidth(minimumWidth()+deltaX);
            mStartPoint=newPos;
            qDebug()<< "minimumWIDTH" + QString::number(minimumWidth());
        }





    }
}
