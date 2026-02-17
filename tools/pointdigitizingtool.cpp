#include "pointdigitizingtool.h"
#include "graphics/mapscene.h"
#include "calculations/unitconverter.h"
vrsa::tools::PointDigitizingTool::PointDigitizingTool(graphics::MapScene *scene, QObject *parent)
    : DigitizingTool{scene, parent}
{

}



void vrsa::tools::PointDigitizingTool::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    auto scenePoint = event->scenePos();
    double pointSize = calculations::UnitConversion::mmToPixels(1);
    double radius = (pointSize / 2.0) / mMapScene->getMapHolderScale();
    mMapScene->addItem(new QGraphicsEllipseItem(scenePoint.x(), scenePoint.y(), radius, radius));
    qDebug()<< "WELL!";
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
}



