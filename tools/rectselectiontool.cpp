#include "rectselectiontool.h"
#include "temporarygraphicsitem.h"
#include "graphics/mapscene.h"

vrsa::tools::RectSelectionTool::RectSelectionTool(graphics::MapScene *scene, QObject *parent)
    : SelectionTool(scene, parent),
      mSelectedFeatureItems{},
      mIsLeftClicked{false},
      mSelectRectItem{nullptr},
      mClickPos{},
      mCurrentPos{}
{
    auto item = std::make_unique<graphics::TemporaryGraphicsItem>(common::GeometryType::Polygon,
                                                                  graphics::TemporaryGraphicsItem::TemporaryItemRole::Selection);
    mSelectRectItem = item.get();
    mSelectRectItem->setZValue(common::MAX_Z_VALUE+1);
    mMapScene->addTemporaryItem(std::move(item));
    mMapScene->setPanningEnable(false);

}

vrsa::tools::RectSelectionTool::~RectSelectionTool()
{
    deselectItems();
}

void vrsa::tools::RectSelectionTool::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (mIsLeftClicked)
            return;
        mSelectRectItem->show();
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

void vrsa::tools::RectSelectionTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    mIsLeftClicked = false;
    mSelectRectItem->hide();
    deselectItems();
    mSelectedFeatureItems.clear();
    auto selectionBoundingRect = mSelectRectItem->boundingRect();
//    qDebug()<<selectionBoundingRect;
    auto sceneItemsInBRect = mMapScene->items(selectionBoundingRect);
    for (const auto& item: sceneItemsInBRect)
    {
        mSelectedFeatureItems.reserve(sceneItemsInBRect.size() - 1); //-1 потому что всегда будем выбирать и себя
        if (auto* featureItem = mMapScene->asFeatureItem(item))
        {
            featureItem->setSelected(true);
            mSelectedFeatureItems.push_back(featureItem);
        }
    }
    emit toolEvent(ToolEventType::MultipleFeaturesSelected, QVariant::fromValue(mSelectedFeatureItems));
}

void vrsa::tools::RectSelectionTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (mIsLeftClicked)
    {
        mCurrentPos = event->scenePos();
        updateGeometry();
    }
}

void vrsa::tools::RectSelectionTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        deselectItems();
    }

}

void vrsa::tools::RectSelectionTool::keyReleaseEvent(QKeyEvent *event)
{

}

void vrsa::tools::RectSelectionTool::updateGeometry()
{

    QRectF rawRect(mClickPos, mCurrentPos);
    QRectF rect = rawRect.normalized();
    auto geom = geometry::rectToGeometry(rect);
    mSelectRectItem->setGeometry(geom);
    mSelectRectItem->update();
}

void vrsa::tools::RectSelectionTool::deselectItems()
{
    for (const auto& item: mSelectedFeatureItems)
    {
        if (item)
            item->setSelected(false);
    }
}


