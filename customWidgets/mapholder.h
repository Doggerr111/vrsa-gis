#ifndef LIPMAPHOLDER_H
#define LIPMAPHOLDER_H

#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "calculations/mapcalculations.h"
#include "ogr_spatialref.h"
/** Данный класс представляет собой виджет для отображения цифровой карты */
class MapHolder : public QGraphicsView
{
    Q_OBJECT
public:
    MapHolder(QObject *parent);
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
    /** Слот вызывается, когда пользователь начинает добавлять объекты на карту (при оцифровке) */
    void onAddingFeatures();
    /** Слот вызывается, когда пользователь прекращает добавлять объекты на карту (при оцифровке) */
    void onStopAddingFeatures();
    /** Слот для изменения состояния добавления объектов на карту (при оцифровке) */
    void updateAddingFeaturesFlag(bool flag);

    void onCrsChanged(vrsa::gdalwrapper::SpatialReference &crs);

    void recalculateScale();

signals:
    /** Сигнал вызывается при изменении размеров виджета*/
    void MapHolderResized();
    /** Сигнал вызывается при изменением пользователем масштаба (при зумировании)*/
    void MapHolderZoomed(double);
    /** Сигнал вызывается при изменением видимого окна карты */
    void extentChanged();

    void CRSChanged(OGRSpatialReference*);

    void scaleChanged(int, double);
    // QWidget interface
protected:
    //слоты для обработки различных событий с виджетом
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;


private:
//    QPointF clickPos;
//    bool isDraging;

    bool isDraging = false;
        QPoint clickPos;
        QPoint m_lastPos;
        QPixmap m_dragPixmap;
        QImage m_dragImage;
        double scaleF;

    bool isAddingFeatures;
    double scaleFactor;
    int mCurrentScale;
    vrsa::calculations::MapCalculator mMapCalculator;
    OGRSpatialReference* mCRS;

};

#endif // LIPMAPHOLDER_H
