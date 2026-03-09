#include "polygondigitizingtool.h"
#include "graphics/mapscene.h"

vrsa::tools::PolygonDigitizingTool::PolygonDigitizingTool(graphics::MapScene *scene, vector::VectorLayer *activeLayer, QObject *parent)
    : DigitizingTool{scene, activeLayer, parent},
      mItem{nullptr}

{
    auto item = std::make_unique<graphics::TemporaryGraphicsItem>(common::GeometryType::Polygon,
                                                                  ItemRole::Digitizing);
    mItem = item.get();
    mMapScene->addTemporaryItem(std::move(item));
    mGeom.type = common::GeometryType::Polygon;
    mGeom.variant = std::vector<QPointF>();
}

void vrsa::tools::PolygonDigitizingTool::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF scenePoint = event->scenePos();
        //фиксируем текущую позицию
        if (points().size() > mFixedCount)
        {
            points().back() = scenePoint;
            mFixedCount = points().size();
        }
        else
        {
            points().push_back(scenePoint);
            mFixedCount = 1;
        }
        mLastScenePos = scenePoint;
        updateGeometry();
        event->accept();
    }
    else if (event->button() == Qt::RightButton)
    {
        if (mFixedCount<3)
        {
            VRSA_DEBUG("PolygonDigitizingTool", "Can't add polygon with less than 3 points");
            return;
        }
        if (points().size() != mFixedCount)
            points().pop_back();

        emit geometryCreated(mGeom);
        clearGeometry();

    }
    else if (event->button() == Qt::MiddleButton)
    {
        //event->ignore();
    }

}

void vrsa::tools::PolygonDigitizingTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF currentPos = event->scenePos();

    if (points().size() == mFixedCount)
    {
        // для резиновой линии (полигона хд??)
        points().push_back(currentPos);
        mFixedCount = points().size() - 1;
    }
    else if (points().size() > mFixedCount)
        points().back() = currentPos;
    updateGeometry();

}

void vrsa::tools::PolygonDigitizingTool::keyPressEvent(QKeyEvent *event)
{
    //todo ctrl+z n esc cancel()
}

QString vrsa::tools::PolygonDigitizingTool::name() const
{
    return tr("Оцифровка линий");
}

QString vrsa::tools::PolygonDigitizingTool::toolTip() const
{
    return tr("С помощью данного инструмента можно оцифровать новый линейный объект слоя");
}

void vrsa::tools::PolygonDigitizingTool::cancel()
{
    //mScene->removeItem(mItem->get())
    //mScene->deselectCurrentMapTool();
}

inline void vrsa::tools::PolygonDigitizingTool::updateGeometry()
{
    mItem->setGeometry(mGeom);
    mItem->update();
}

inline void vrsa::tools::PolygonDigitizingTool::clearGeometry()
{
    mGeom.variant = std::vector<QPointF>();
    updateGeometry();
}


