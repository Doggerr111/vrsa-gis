#ifndef LIPMAPHOLDER_H
#define LIPMAPHOLDER_H

#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "calculations/mapcalculations.h"
#include "ogr_spatialref.h"
/**
 * @english
 * @brief Widget for displaying digital map with pan and zoom capabilities
 *
 * MapHolder extends QGraphicsView to provide a digital map view with:
 * - Smooth panning using snapshots (drag image)
 * - Smooth zooming using snapshots (zoom preview)
 * - Scale calculation and display
 * - Mouse interaction for panning and zooming
 *
 * @russian
 * @brief Виджет для отображения цифровой карты с поддержкой панорамирования и масштабирования
 *
 * MapHolder расширяет QGraphicsView для отображения цифровой карты с возможностью:
 * - Плавное панорамирование с использованием снимков (drag image)
 * - Плавное масштабирование с использованием снимков (zoom preview)
 * - Расчет и отображение текущего масштаба
 * - Взаимодействие мышью для перемещения и масштабирования
 */
class MapHolder : public QGraphicsView
{
    Q_OBJECT
public:
    MapHolder(QWidget *parent);
    /** Смещает видимую область виджета в bRect */
    void zoomToRect(QRectF bRect);
    /** Возвращает текущий масштаб */
    int getScale();
    /** Возвращает View->transform()->m11() */
    double getMapHolderScale() const;
    /** Возвращает текущую видимую область виджета */
    QRectF getExtent();

    void SetSpatialRef(OGRSpatialReference* ref);
    void setMapCalculator(vrsa::calculations::MapCalculator& calc);

public slots:
    void onCrsChanged(vrsa::spatialref::SpatialReference &crs);
    void recalculateScale();
    //для блокирования перемещения по сцене
    void onPanningRequested(bool enable) { mPanningEnabled = enable; };

signals:
    /** Сигнал вызывается при изменении размеров виджета*/
    void MapHolderResized();
    /** Сигнал вызывается при изменением пользователем масштаба (при зумировании)*/
    void MapHolderZoomed(double);
    /** Сигнал вызывается при изменением видимого окна карты */
    void extentChanged();

    void CRSChanged(OGRSpatialReference*);

    void scaleChanged(int, double);

    //для перехвата его быстрее view в сцене, чтобы например заблокировать перемещение сцены
    void mousePressed(QMouseEvent *event);
    // QWidget interface
protected:
    //слоты для обработки различных событий с виджетом
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    //zooming

    void startZoomSnapshot(const QPointF& mousePos);
    void handleZoom(QWheelEvent* event);
    void finishZoom();
    void updateZoomPreview();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;


private:

    bool mIsDraging = false;
    bool mIsZooming = false;
    double mZoomDelta;
    QPointF mZoomOffset;
    QTimer* mZoomTimer = nullptr;
    QPixmap mZoomPixmap;
    double mZoomStartScale = 1.0;
    QPointF mZoomMousePos;
    QPoint mClickPos;
    QPoint mLastPos;
    QPixmap mDragPixmap;
    QImage mDragImage;
    double scaleF;
    double mZoomInFactor = 1.2;
    double mZoomOutFactor = 0.8;
    int mCurrentScale;
    vrsa::calculations::MapCalculator mMapCalculator;
    OGRSpatialReference* mCRS;
    bool mPanningEnabled{true};
};

#endif // LIPMAPHOLDER_H
