#ifndef LIPMAPHOLDER_H
#define LIPMAPHOLDER_H

#include <QGraphicsView>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>
//#include "lipmapcalculations.h"
/** Данный класс представляет собой виджет для отображения цифровой карты */
class LIPMapHolder : public QGraphicsView
{
    Q_OBJECT
public:
    LIPMapHolder(QObject *parent);
    /** Смещает видимую область виджета в bRect */
    void zoomToRect(QRectF bRect);
    /** Возвращает текущий масштаб */
    int getScale();
    /** Возвращает текущую видимую область виджета */
    QRectF getExtent();

public slots:
    /** Слот вызывается, когда пользователь начинает добавлять объекты на карту (при оцифровке) */
    void onAddingFeatures();
    /** Слот вызывается, когда пользователь прекращает добавлять объекты на карту (при оцифровке) */
    void onStopAddingFeatures();
    /** Слот для изменения состояния добавления объектов на карту (при оцифровке) */
    void updateAddingFeaturesFlag(bool flag);

signals:
    /** Сигнал вызывается при изменении размеров виджета*/
    void MapHolderResized();
    /** Сигнал вызывается при изменением пользователем масштаба (при зумировании)*/
    void MapHolderZoomed(double);
    /** Сигнал вызывается при изменением видимого окна карты */
    void extentChanged();

    // QWidget interface
protected:
    //слоты для обработки различных событий с виджетом
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


private:
    QPointF clickPos;
    bool isDraging;
    bool isAddingFeatures;
    double scaleFactor;
};

#endif // LIPMAPHOLDER_H
