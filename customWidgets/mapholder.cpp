#include "mapholder.h"
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QElapsedTimer>
//#include "QOpenGLWidget"
#include <QTimer>
MapHolder::MapHolder(QWidget *parent)
    : QGraphicsView{parent},
      mMapCalculator{}
{
    mMapCalculator.SetDpi(QGuiApplication::primaryScreen()->logicalDotsPerInch());
    setOptimizationFlags( QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setCacheMode(QGraphicsView::CacheNone);
    mZoomTimer = new QTimer(this);
    mZoomTimer->setSingleShot(true);
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


void MapHolder::onCrsChanged(vrsa::spatialref::SpatialReference &crs)
{
    mMapCalculator.setCRS(crs);
    recalculateScale();
}

void MapHolder::recalculateScale()
{
    const QRectF visibleRect = mapToScene(viewport()->rect()).boundingRect();
    mCurrentScale = static_cast<int>(mMapCalculator.calculate(visibleRect, width()));
    //VRSA_DEBUG("MapHolder", "Calculated scale:" + std::to_string(mCurrentScale));
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
    mIsDraging = false;
    if (!mIsZooming)
    {
        //создаем снапшот
        startZoomSnapshot(event->position());
        mIsZooming = true;
    }
    // сразу применяем зум к сцене
    qreal factor = (event->angleDelta().y() > 0) ? mZoomInFactor : mZoomOutFactor;
    scale(factor, factor);
    mZoomDelta += event->angleDelta().y();
    //обновляем снапшот
    handleZoom(event);
    recalculateScale();
    if (mZoomTimer)
    {
        mZoomTimer->disconnect();  // отключаем старый
        connect(mZoomTimer, &QTimer::timeout, this, &MapHolder::finishZoom, Qt::UniqueConnection);
        mZoomTimer->stop();
        mZoomTimer->start(300);
    }

    viewport()->update();

    //    if (event->angleDelta().y()<0 )
    //    {
    //        scaleFactor=0.3;

    //        scale(scaleFactor, scaleFactor);
    //    }
    //    else if (event->angleDelta().y()>0) {
    //        scaleFactor=2;
    //        scale(scaleFactor, scaleFactor);
    //    }
    //    recalculateScale();
}

void MapHolder::startZoomSnapshot(const QPointF &mousePos)
{
    mIsZooming = true;
    mZoomMousePos = mousePos;
    mZoomStartScale = transform().m11();  // текущий масштаб
    mZoomOffset = QPointF(0, 0); //левый верхний угол в координатах вьюпорта
    //для скорости
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    //рендерим снапшот
    mZoomPixmap = QPixmap(viewport()->size());
    mZoomPixmap.fill(Qt::transparent);
    QPainter painter(&mZoomPixmap);
    render(&painter);
    painter.end();
    //возвращаем
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
}

void MapHolder::handleZoom(QWheelEvent *event)
{
    //коэффициент зума
    qreal zoomFactor = (event->angleDelta().y() > 0) ? mZoomInFactor : mZoomOutFactor;
    // обновляем снепшот
    QSize newSize = mZoomPixmap.size() * zoomFactor;
    //защита от слишком большого или слишком маленького размера
    if (newSize.width() > 5000 || newSize.height() > 5000) {
        finishZoom();
        return;
    }
    if (newSize.width() < 10 || newSize.height() < 10) {
        finishZoom();
        return;
    }

    mZoomOffset = (mZoomOffset - mZoomMousePos) * zoomFactor + mZoomMousePos;
    mZoomPixmap = mZoomPixmap.scaled(
                mZoomPixmap.size() * zoomFactor,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
    viewport()->update();
}


void MapHolder::finishZoom()
{
    if (!mIsZooming) return;
    mIsZooming = false;
    mZoomPixmap = QPixmap();
    mZoomDelta = 0;
    recalculateScale();
    viewport()->update();
}

void MapHolder::mousePressEvent(QMouseEvent* event) {
    //чтобы знать можно ли перемещать или нет
    emit mousePressed(event);
    if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton)
    {
        if (!mPanningEnabled && event->button() == Qt::LeftButton)
        {

            mIsDraging = false;
            QGraphicsView::mousePressEvent(event);
            return;
        }

        mIsDraging = true;
        mClickPos = event->pos();
        mLastPos = event->pos();

        // Отключаем улучшения
        setRenderHint(QPainter::Antialiasing, false);
        setRenderHint(QPainter::SmoothPixmapTransform, false);

        // Замер времени начала
        QElapsedTimer timer;
        timer.start();

        // Рендерим в QImage (быстрее!)
        mDragImage = QImage(viewport()->size(), QImage::Format_ARGB32_Premultiplied);
        mDragImage.fill(Qt::transparent);

        QPainter painter(&mDragImage);
        render(&painter);
        painter.end();

        // Замер времени окончания
        qint64 elapsed = timer.elapsed();
        qDebug() << "Время рендера drag-изображения:" << elapsed << "мс";

        // Конвертируем в QPixmap для отрисовки
        mDragPixmap = QPixmap::fromImage(mDragImage);

        // Возвращаем настройки
        setRenderHint(QPainter::Antialiasing, true);
        setRenderHint(QPainter::SmoothPixmapTransform, true);

        viewport()->update();
    }
    QGraphicsView::mousePressEvent(event);
}

void MapHolder::mouseMoveEvent(QMouseEvent* event) {
    if (mIsDraging) {
        mLastPos = event->pos();
        viewport()->update();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MapHolder::mouseReleaseEvent(QMouseEvent* event)
{
    if (mIsDraging)
    {
        mIsDraging = false;

        // Сдвигаем сцену
        QPointF start = mapToScene(mClickPos);
        QPointF end = mapToScene(mLastPos);
        QPointF delta = end - start;

        QPointF newCenter = mapToScene(viewport()->rect().center()) - delta;
        centerOn(newCenter);

        mDragPixmap = QPixmap();
        viewport()->update();
    }
    if (mIsZooming)
    {
        qDebug()<<"zooming stopped";
        mIsZooming = false;
        mZoomPixmap = QPixmap();
        QGraphicsView::mouseReleaseEvent(event);
        viewport()->update();
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MapHolder::paintEvent(QPaintEvent* event)
{
    if (mIsZooming && !mZoomPixmap.isNull())
    {
        // Рисуем снапшот
        QPainter painter(viewport());
        QPoint offset(
                    (viewport()->width() - mZoomPixmap.width()) / 2,
                    (viewport()->height() - mZoomPixmap.height()) / 2
                    );

        painter.drawPixmap(mZoomOffset, mZoomPixmap);
        return;
    }

    if (mIsDraging && !mDragPixmap.isNull()) {
        QPainter painter(viewport());
        //painter.fillRect(viewport()->rect(), Qt::lightGray);
        QPoint offset = mLastPos - mClickPos;
        painter.drawPixmap(offset, mDragPixmap);
        return;
    }
    QGraphicsView::paintEvent(event);
}

