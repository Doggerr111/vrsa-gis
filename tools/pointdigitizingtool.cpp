#include "pointdigitizingtool.h"
#include "graphics/mapscene.h"
vrsa::tools::PointDigitizingTool::PointDigitizingTool(graphics::MapScene *scene,
                                                      vector::VectorLayer* activeLayer, QObject *parent)
    : DigitizingTool{scene, activeLayer, parent}
{

}



void vrsa::tools::PointDigitizingTool::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF scenePoint = event->scenePos();
    //qDebug()<<scenePoint;
    vrsa::geometry::Geometry geometry;
    geometry.type = common::GeometryType::Point;
    geometry.variant = scenePoint;
    emit geometryCreated(geometry);
}

void vrsa::tools::PointDigitizingTool::keyPressEvent(QKeyEvent *event)
{

}

QString vrsa::tools::PointDigitizingTool::name() const
{
    return tr("Оцифровка точек");
}

QString vrsa::tools::PointDigitizingTool::toolTip() const
{
    return tr("С помощью данного инструмента можно оцифровать новый точечный объект слоя");
}


void vrsa::tools::PointDigitizingTool::cancel()
{
    auto f = 1;
}



