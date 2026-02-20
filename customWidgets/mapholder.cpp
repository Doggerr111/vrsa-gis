#include "mapholder.h"
#include <QDebug>
#include "QGuiApplication"
#include "QScreen"
#include "QElapsedTimer"
MapHolder::MapHolder(QObject *parent)
    : QGraphicsView{},
      //isDraging{false},
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

double MapHolder::getMapHolderScale() const
{
    return transform().m11();
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

void MapHolder::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDraging = true;
        clickPos = event->pos();
        m_lastPos = event->pos();

        // Отключаем улучшения
        setRenderHint(QPainter::Antialiasing, false);
        setRenderHint(QPainter::SmoothPixmapTransform, false);

        // Замер времени начала
        QElapsedTimer timer;
        timer.start();

        // Рендерим в QImage (быстрее!)
        m_dragImage = QImage(viewport()->size(), QImage::Format_ARGB32_Premultiplied);
        m_dragImage.fill(Qt::transparent);

        QPainter painter(&m_dragImage);
        render(&painter);
        painter.end();

        // Замер времени окончания
        qint64 elapsed = timer.elapsed();
        qDebug() << "Время рендера drag-изображения:" << elapsed << "мс";

        // Конвертируем в QPixmap для отрисовки
        m_dragPixmap = QPixmap::fromImage(m_dragImage);

        // Возвращаем настройки
        setRenderHint(QPainter::Antialiasing, true);
        setRenderHint(QPainter::SmoothPixmapTransform, true);

        viewport()->update();
    }
    QGraphicsView::mousePressEvent(event);
}

void MapHolder::mouseMoveEvent(QMouseEvent* event) {
    if (isDraging) {
        m_lastPos = event->pos();
        viewport()->update();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MapHolder::mouseReleaseEvent(QMouseEvent* event) {
    if (isDraging) {
        isDraging = false;

        // Сдвигаем сцену
        QPointF start = mapToScene(clickPos);
        QPointF end = mapToScene(m_lastPos);
        QPointF delta = end - start;

        QPointF newCenter = mapToScene(viewport()->rect().center()) - delta;
        centerOn(newCenter);

        m_dragPixmap = QPixmap();
        viewport()->update();
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MapHolder::paintEvent(QPaintEvent* event) {
    if (isDraging && !m_dragPixmap.isNull()) {
        QPainter painter(viewport());
        //painter.fillRect(viewport()->rect(), Qt::lightGray);
        QPoint offset = m_lastPos - clickPos;
        painter.drawPixmap(offset, m_dragPixmap);
        return;
    }
    QGraphicsView::paintEvent(event);
}

