#include "mapholder.h"
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QElapsedTimer>
//#include "QOpenGLWidget"
#include <QTimer>
MapHolder::MapHolder(QWidget *parent)
    : QGraphicsView{parent},
      mIsDraging{false},
      mIsZooming{false},
      mZoomDelta{0.0},
      mZoomOffset{},
      mZoomPixmap{},
      mZoomStartScale{1.0},
      mZoomMousePos{},
      mClickPos{},
      mLastPos{},
      mDragPixmap{},
      mDragImage{},
      mZoomInFactor{1.2},
      mZoomOutFactor{0.8},
      mCurrentScale{1},
      mPanningEnabled{true},
      mZoomTimer{std::make_unique<QTimer>()}
{
    setOptimizationFlags (QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setCacheMode(QGraphicsView::CacheNone);
    mZoomTimer->setSingleShot(true);
    scale(1,-1);
}

void MapHolder::zoomToRect(const QRectF &rect)
{
    zoomToPoint(rect.center());
}

void MapHolder::zoomToPoint(const QPointF &point)
{
    centerOn(point);
    emit zoomed();
    emit extentChanged(getExtent(), viewport()->rect());
}

void MapHolder::zoomIn()
{
    qreal factor = mZoomInFactor;
    double futureScale = mCurrentScale / factor;
    static const double MIN_SCALE = 100.0;      // 1:100 - максимальный зум
    static const double MAX_SCALE = 1000000000.0; //минимальный зум
    if (futureScale < MIN_SCALE) return;
    else if (futureScale > MAX_SCALE) return;
    if (qFuzzyCompare(mCurrentScale, futureScale)) return;
    if (futureScale >= MIN_SCALE && futureScale <= MAX_SCALE)
    {
        scale(factor, factor);
        emit zoomed();
        viewport()->update();
        emit extentChanged(getExtent(), viewport()->rect());
    }

}

void MapHolder::zoomOut()
{
    qreal factor = mZoomOutFactor;
    double futureScale = mCurrentScale / factor;
    static const double MIN_SCALE = 100.0;      // 1:100 - максимальный зум
    static const double MAX_SCALE = 1000000000.0; //минимальный зум
    if (futureScale < MIN_SCALE) return;
    else if (futureScale > MAX_SCALE) return;
    if (qFuzzyCompare(mCurrentScale, futureScale)) return;
    if (futureScale >= MIN_SCALE && futureScale <= MAX_SCALE)
    {
        scale(factor, factor);
        emit zoomed();
        viewport()->update();
        emit extentChanged(getExtent(), viewport()->rect());
    }
}

double MapHolder::getMapHolderScale() const
{
    return transform().m11();
}

QRectF MapHolder::getExtent() const
{
    if (!viewport()) return QRectF();
    QPolygonF scenePoly = mapToScene(viewport()->rect());
    QRectF sceneRect = scenePoly.boundingRect();
    double minX = sceneRect.left();
    double maxX = sceneRect.right();
    double minY = sceneRect.bottom();  // юг
    double maxY = sceneRect.top();     // север
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

QRectF MapHolder::getSceneRect() const
{
    return mapToScene(viewport()->rect()).boundingRect();
}

void MapHolder::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    emit extentChanged(getExtent(), viewport()->rect());
    emit resized();
}

void MapHolder::wheelEvent(QWheelEvent *event)
{
    mIsDraging = false;

    qreal factor = (event->angleDelta().y() > 0) ? mZoomInFactor : mZoomOutFactor;
    //предварительно рассчитываем будущий масштаб
    double futureScale = mCurrentScale / factor;
    static const double MIN_SCALE = 100.0;      // 1:100 - максимальный зум
    static const double MAX_SCALE = 1000000000.0; //минимальный зум
    if (futureScale < MIN_SCALE) return;
    else if (futureScale > MAX_SCALE) return;
    //применяем зум только если масштаб реально изменился
    if (qFuzzyCompare(mCurrentScale, futureScale)) return;

    if (futureScale >= MIN_SCALE && futureScale <= MAX_SCALE)
    {
        if (!mIsZooming)
        {
            //создаем снапшот
            startZoomSnapshot(event->position());
            mIsZooming = true;
        }
        //сразу применяем зум к сцене
        scale(factor, factor);
        mZoomDelta += event->angleDelta().y();
        handleZoom(event);  //обновляем снапшот
        emit zoomed();
        if (mZoomTimer)
        {
            mZoomTimer->disconnect();  // отключаем старый
            connect(mZoomTimer.get(), &QTimer::timeout, this, &MapHolder::finishZoom, Qt::UniqueConnection);
            mZoomTimer->stop();
            mZoomTimer->start(300);
        }
        viewport()->update();
        emit extentChanged(getExtent(), viewport()->rect());
        return;
    }

}

void MapHolder::startZoomSnapshot(const QPointF &mousePos)
{
    mIsZooming = true;
    mZoomMousePos = mousePos;
    mZoomStartScale = transform().m11();  // текущий масштаб
    mZoomOffset = QPointF(0, 0); //левый верхний угол в координатах вьюпорта
    //для более быстрой отрисовки
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    //рендерим снапшот
    mZoomPixmap = QPixmap(viewport()->size());
    mZoomPixmap.fill(Qt::transparent);
    QPainter painter(&mZoomPixmap);
    render(&painter);
    painter.end();
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
    if (newSize.width() > 5000 || newSize.height() > 5000)
    {
        finishZoom();
        return;
    }
    if (newSize.width() < 10 || newSize.height() < 10)
    {
        finishZoom();
        return;
    }
    mZoomOffset = (mZoomOffset - mZoomMousePos) * zoomFactor + mZoomMousePos;
    mZoomPixmap = mZoomPixmap.scaled( mZoomPixmap.size() * zoomFactor,
                       Qt::KeepAspectRatio, Qt::SmoothTransformation);
    viewport()->update();
}


void MapHolder::finishZoom()
{
    if (!mIsZooming) return;
    mIsZooming = false;
    mZoomPixmap = QPixmap();
    mZoomDelta = 0;
    emit zoomed();
    viewport()->update();
}

void MapHolder::mousePressEvent(QMouseEvent* event)
{
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
        //для более быстрой отрисовки
        setRenderHint(QPainter::Antialiasing, false);
        setRenderHint(QPainter::SmoothPixmapTransform, false);
        //замер времени отрисовки
        QElapsedTimer timer;
        timer.start();
        //рендер в QImage
        mDragImage = QImage(viewport()->size(), QImage::Format_ARGB32_Premultiplied);
        mDragImage.fill(Qt::transparent);
        QPainter painter(&mDragImage);
        render(&painter);
        painter.end();
        qint64 elapsed = timer.elapsed();
        qDebug() << "Время рендера drag-изображения:" << elapsed << "мс";
        //конвертируем в QPixmap для отрисовки
        mDragPixmap = QPixmap::fromImage(mDragImage);
        setRenderHint(QPainter::Antialiasing, true);
        setRenderHint(QPainter::SmoothPixmapTransform, true);
        viewport()->update();
    }
    QGraphicsView::mousePressEvent(event);
}

void MapHolder::mouseMoveEvent(QMouseEvent* event)
{
    if (mIsDraging)
    {
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
        //сдвигаем сцену
        QPointF start = mapToScene(mClickPos);
        QPointF end = mapToScene(mLastPos);
        QPointF delta = end - start;
        QPointF newCenter = mapToScene(viewport()->rect().center()) - delta;
        centerOn(newCenter);
        mDragPixmap = QPixmap();
        viewport()->update();
        emit extentChanged(getExtent(), viewport()->rect());
    }
    if (mIsZooming)
    {
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
        //рисуем снапшот
        QPainter painter(viewport());
        QPoint offset((viewport()->width() - mZoomPixmap.width()) / 2,
                      (viewport()->height() - mZoomPixmap.height()) / 2);
        painter.drawPixmap(mZoomOffset, mZoomPixmap);
        return;
    }

    if (mIsDraging && !mDragPixmap.isNull())
    {
        QPainter painter(viewport());
        QPoint offset = mLastPos - mClickPos;
        painter.drawPixmap(offset, mDragPixmap);
        return;
    }
    QGraphicsView::paintEvent(event);
}

