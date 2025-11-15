#include "mapholder.h"
#include <QDebug>
#include "QGuiApplication"
#include "QScreen"
MapHolder::MapHolder(QObject *parent)
    : QGraphicsView{},
      isDraging{false},
      isAddingFeatures{false},
      mMapCalculator{}
{
    Q_UNUSED(parent);
    mMapCalculator.SetDpi(QGuiApplication::primaryScreen()->logicalDotsPerInch());
}

void MapHolder::zoomToRect(QRectF targetRect)
{
    Q_UNUSED(targetRect);
}

int MapHolder::getScale()
{
    QMatrix const matrix = this->matrix().inverted();
    QRectF visibleRect = matrix.mapRect((viewport()->rect()));
    visibleRect.moveTopLeft(matrix.map(QPoint(horizontalScrollBar()->value(),
                                              verticalScrollBar()->value())));
    QRectF visibleRect1 = mapToScene(viewport()->rect()).boundingRect();
    double scale = mMapCalculator.calculate(visibleRect1, width());
    return static_cast<int>(scale);
}

QRectF MapHolder::getExtent()
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
    qDebug()<<visibleRect;
    return visibleRect;
}

void MapHolder::setMapCalculator(vrsa::calculations::MapCalculator &calc)
{
    mMapCalculator=calc;
}

void MapHolder::onAddingFeatures()
{
    isAddingFeatures=true;
}

void MapHolder::onStopAddingFeatures()
{
    isAddingFeatures=false;
}

void MapHolder::updateAddingFeaturesFlag(bool flag)
{
    isAddingFeatures=flag;
}

void MapHolder::onCrsChanged(vrsa::gdalwrapper::SpatialReference &crs)
{
    mMapCalculator.setCRS(crs);
}

void MapHolder::recalculateScale()
{
    const QRectF visibleRect = mapToScene(viewport()->rect()).boundingRect();
    mCurrentScale = static_cast<int>(mMapCalculator.calculate(visibleRect, width()));
    VRSA_DEBUG("MapHolder", "Calculated scale:" + std::to_string(mCurrentScale));
    emit scaleChanged(mCurrentScale, transform().m11());
}

void MapHolder::resizeEvent(QResizeEvent *event)
{
    emit MapHolderResized();
    QGraphicsView::resizeEvent(event);
    recalculateScale();

}

void MapHolder::wheelEvent(QWheelEvent *event)
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
    recalculateScale();
    //emit MapHolderZoomed(scaleFactor);
    //QTransform currentTransform = transform();
    //qreal scaleX = currentTransform.m11(); // Масштаб по горизонтали
    //qreal scaleY = currentTransform.m22();
    //qDebug()<<scaleFactor<<scaleX<<scaleY;
    //QGraphicsView::wheelEvent(event);
}

void MapHolder::mousePressEvent(QMouseEvent *event)
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

void MapHolder::mouseReleaseEvent(QMouseEvent *event)
{

    isDraging=false;
    QGraphicsView::mouseReleaseEvent(event);
}

void MapHolder::mouseMoveEvent(QMouseEvent *event)
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


