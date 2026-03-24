#include "rulertool.h"
#include "temporarygraphicsitem.h"
#include "graphics/mapscene.h"

vrsa::tools::RulerTool::RulerTool(graphics::MapScene *scene, QObject *parent)
    : MapTool(scene, parent),
      mIsLeftClicked{false},
      mRulerItem{nullptr},
      mClickPos{},
      mCurrentPos{}
{
    auto item = std::make_unique<graphics::TemporaryGraphicsItem>(common::GeometryType::LineString,
                                    graphics::TemporaryGraphicsItem::TemporaryItemRole::Measurement);
    mRulerItem = item.get();
    mRulerItem->setZValue(common::MAX_Z_VALUE+1);
    mMapScene->addTemporaryItem(std::move(item));
    mMapScene->setPanningEnable(false);

}

void vrsa::tools::RulerTool::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (mIsLeftClicked)
        {
            mIsLeftClicked = false;
            return;
        }
        mRulerItem->show();
        mClickPos = event->scenePos();
        mCurrentPos = event->scenePos();
        mIsLeftClicked = true;
        updateGeometry();
    }
    else if (event->button() == Qt::RightButton)
    {
        mIsLeftClicked = false;
    }
}

void vrsa::tools::RulerTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //mIsLeftClicked = false;
    //emit toolEvent(ToolEventType::MeasurementRequired, QVariant::fromValue(mSelectedFeatureItems));
}

void vrsa::tools::RulerTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (mIsLeftClicked)
    {
        mCurrentPos = event->scenePos();
        emit toolEvent(ToolEventType::SimpleLineRulerGeometryChanged,
                       QVariant::fromValue(std::vector{mClickPos, mCurrentPos}));
        updateGeometry();
    }
}

void vrsa::tools::RulerTool::keyPressEvent(QKeyEvent *event)
{

}

void vrsa::tools::RulerTool::updateGeometry()
{
    auto geom = geometry::Geometry();
    geom.type = common::GeometryType::LineString;
    geom.variant = std::vector{mClickPos, mCurrentPos};
    mRulerItem->setGeometry(geom);
    mRulerItem->update();
}

vrsa::tools::RulerTool::~RulerTool() = default;
