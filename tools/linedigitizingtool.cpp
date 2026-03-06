#include "linedigitizingtool.h"
#include "graphics/mapscene.h"
vrsa::tools::LineDigitizingTool::LineDigitizingTool(graphics::MapScene *scene, QObject *parent)
    : DigitizingTool{scene, parent},
      mItem{nullptr}

{
    auto item = std::make_unique<graphics::TemporaryGraphicsItem>(common::GeometryType::LineString,
                                                                  ItemRole::Digitizing);
    mItem = item.get();
    mMapScene->addTemporaryItem(std::move(item));
    mGeom.type = common::GeometryType::LineString;
    mGeom.variant = std::vector<QPointF>();
}

void vrsa::tools::LineDigitizingTool::mousePressEvent(QGraphicsSceneMouseEvent *event)
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
        if (mFixedCount<2)
        {
            VRSA_DEBUG("LineDigitizingTool", "Can't add line with less than 2 points");
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

void vrsa::tools::LineDigitizingTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF currentPos = event->scenePos();


    //если есть только зафиксированные точки
    if (points().size() == mFixedCount)
    {
        // добавляем временную точку для резиновой линии
        points().push_back(currentPos);
        mFixedCount = points().size() - 1;
    }

    else if (points().size() > mFixedCount)
        points().back() = currentPos;
    updateGeometry();

}

void vrsa::tools::LineDigitizingTool::keyPressEvent(QKeyEvent *event)
{
    //todo ctrl+z n esc cancel()
}

QString vrsa::tools::LineDigitizingTool::name() const
{
    return tr("Оцифровка линий");
}

QString vrsa::tools::LineDigitizingTool::toolTip() const
{
    return tr("С помощью данного инструмента можно оцифровать новый линейный объект слоя");
}

void vrsa::tools::LineDigitizingTool::cancel()
{
    //mScene->removeItem(mItem->get())
    //mScene->deselectCurrentMapTool();
}

inline void vrsa::tools::LineDigitizingTool::updateGeometry()
{
    mItem->setGeometry(mGeom);
}

inline void vrsa::tools::LineDigitizingTool::clearGeometry()
{
    mGeom.variant = std::vector<QPointF>();
    updateGeometry();
}


