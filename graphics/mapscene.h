#ifndef LIPMAPSCENE_H
#define LIPMAPSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
//#include "vector/lippointlayer.h"
//#include "vector/lippointgraphicsitem.h"
//#include "vector/liplinelayer.h"
//#include "vector/liplinegraphicsitem.h"
//#include "vector/lippolygonlayer.h"
//#include "vector/lippolygongraphicsitem.h"
#include "QGraphicsLineItem"
#include "QGraphicsPolygonItem"
//#include "lipnewlinelayerform.h"
//#include "lipnewattrfeatureform.h"
//#include "vector/lipvectorconvertor.h"
#include <QKeyEvent>

namespace vrsa
{
namespace graphics
{


class MapScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MapScene(QObject *parent = nullptr);


    //void startAddingFeatures(LIPVectorLayer *activeLayer);
    //void stopAddingFeatures();
    // QGraphicsScene interface
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
signals:
    void mouseMoved(QPointF);
//    void pos_changed(QPointF);
//    void scene_dragging(QPointF,QPointF);
//    /** Сигнал, срабатывающий, когда добавляются новые объекты к слою*/
//    void startAdding();
//    void stopAdding();


    // QGraphicsScene interface
protected:
    void wheelEvent(QGraphicsSceneWheelEvent *event);

    // QGraphicsScene interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
public slots:
//    void drawVectorLayer(LIPVectorLayer*);
//    void redrawVectorLayer(LIPVectorLayer*);
//    void updateVectorLayer();
//    void addPointFeature();
private:
    QPointF clickPos;
    bool isDraging;
    bool isAddingFeaturesToMap;
    bool isPoint;
    bool isLine; //флаг для проверки является ли активный слой линейным
    bool isPolygon;
    QVector<QPointF> vectPoints;
    //элементы, отображаемые при добавлении на карту новых объектов.
    //LIPLineGraphicsItem *tempLine;
    //LIPLineGraphicsItem * lineItem;
    QGraphicsPolygonItem *tempPoly=nullptr;
    //LIPVectorLayer *activeLayer;
    //QVector<LIPPointLayer*> layers;

    // QGraphicsScene interface
protected:
    void keyPressEvent(QKeyEvent *event);
};

}
}

#endif // LIPMAPSCENE_H
