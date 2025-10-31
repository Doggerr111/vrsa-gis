#include "lipmapholder.h"
#include <QDebug>
LIPMapHolder::LIPMapHolder(QObject *parent)
    : QGraphicsView{},
      isDraging{false},
      isAddingFeatures{false}
{
    Q_UNUSED(parent);
}

void LIPMapHolder::zoomToRect(QRectF targetRect)
{
    Q_UNUSED(targetRect);
}

int LIPMapHolder::getScale()
{
//    LIPMapCalculations *calculator = new LIPMapCalculations();
//    calculator->setDpi(QGuiApplication::primaryScreen()->logicalDotsPerInch());
//    QMatrix const matrix = this->matrix().inverted();
//    QRectF visibleRect = matrix.mapRect((viewport()->rect()));
//    visibleRect.moveTopLeft(matrix.map(QPoint(horizontalScrollBar()->value(),
//                                              verticalScrollBar()->value())));
//    QRectF visibleRect1 = mapToScene(viewport()->rect()).boundingRect();
//    double scale = calculator->calculate(visibleRect1, width());
//    delete calculator;
//    return static_cast<int>(scale);
}

QRectF LIPMapHolder::getExtent()
{
    QMatrix const matrix = this->matrix().inverted();

    QRectF visibleRect = matrix.mapRect((viewport()->rect()));
    visibleRect.moveTopLeft(matrix.map(QPoint(horizontalScrollBar()->value(),
                                              verticalScrollBar()->value())));
    qDebug()<<visibleRect;
    double topLat;
    double leftLon;
    double bottomLat;
    double rightLon;
    visibleRect.getRect(&topLat, &leftLon, &bottomLat, &rightLon);
    bottomLat = topLat + bottomLat;
    rightLon = leftLon - rightLon;
    visibleRect.setWidth(bottomLat);
    visibleRect.setHeight(rightLon);
//    qDebug()<<"extent"<<QString::number(topLat,'f',3)<<QString::number(leftLon,'f',3)
//           <<QString::number(bottomLat,'f',3)<<QString::number(rightLon,'f',3);
    qDebug()<<visibleRect;
    return visibleRect;
}

void LIPMapHolder::onAddingFeatures()
{
    isAddingFeatures=true;
}

void LIPMapHolder::onStopAddingFeatures()
{
    isAddingFeatures=false;
}

void LIPMapHolder::updateAddingFeaturesFlag(bool flag)
{
    isAddingFeatures=flag;
}

void LIPMapHolder::resizeEvent(QResizeEvent *event)
{
    emit MapHolderResized();
    QGraphicsView::resizeEvent(event);

}

void LIPMapHolder::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y()<0 )
    {
        scaleFactor=0.3;

       scale(scaleFactor, scaleFactor);
    }
    else if (event->angleDelta().y()>0) {
        scaleFactor=2;
        scale(scaleFactor, scaleFactor);
    }
    //emit MapHolderZoomed(scaleFactor);
    //QTransform currentTransform = transform();
    //qreal scaleX = currentTransform.m11(); // Масштаб по горизонтали
    //qreal scaleY = currentTransform.m22();
    //qDebug()<<scaleFactor<<scaleX<<scaleY;
    //QGraphicsView::wheelEvent(event);
}

void LIPMapHolder::mousePressEvent(QMouseEvent *event)
{

    if (event->button()==Qt::LeftButton && !isAddingFeatures)
        isDraging=true;
    else if (event->button()==Qt::MiddleButton && isAddingFeatures)
        isDraging=true;
    //clickPos=mapToScene(event->pos());
    clickPos=event->pos();
    qDebug()<<isDraging;
    QGraphicsView::mousePressEvent(event);
//    qDebug()<<clickPos;
//    qDebug()<<mapToScene(event->pos());
}

void LIPMapHolder::mouseReleaseEvent(QMouseEvent *event)
{

    isDraging=false;
    QGraphicsView::mouseReleaseEvent(event);
}

void LIPMapHolder::mouseMoveEvent(QMouseEvent *event)
{

    if (isDraging)
    {
//        qDebug()<<"координаты клика";
//        qDebug()<<clickPos;
//        QPointF delta=clickPos-mapToScene(event->pos());
//        clickPos=mapToScene(event->pos());
//        qDebug()<<"координаты перемещени";
//        qDebug()<<clickPos;
//        QPoint rect=viewport()->rect().center();
//        qDebug()<<"координаты";
//        qDebug()<<mapToScene(this->viewport()->rect().center())+delta.toPoint();
//        this->centerOn(mapToScene(this->viewport()->rect().center())+delta.toPoint());


        QPointF delta=clickPos-event->pos();
        clickPos=event->pos();
        //QPoint rect=viewport()->rect().center();
        this->centerOn(mapToScene(this->viewport()->rect().center()+delta.toPoint()));
        emit extentChanged();
    }
    QGraphicsView::mouseMoveEvent(event);
}


