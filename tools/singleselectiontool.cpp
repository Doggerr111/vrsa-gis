#include "singleselectiontool.h"
#include "mapscene.h"


vrsa::tools::SingleSelectionTool::~SingleSelectionTool()
{
    if (mCurrentSelectedFeatureItem)
        mCurrentSelectedFeatureItem->setSelected(false);
}

void vrsa::tools::SingleSelectionTool::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!mMapScene)
        return;
    QGraphicsItem* clickedItem = mMapScene->itemAt(event->scenePos(), QTransform());
    if (!clickedItem) return;
    if (auto* featureItem = mMapScene->asFeatureItem(clickedItem))
    {
        selectVectorFeature(featureItem);
        emit toolEvent(ToolEventType::FeatureSelected, QVariant::fromValue(featureItem));
    }
    else if (auto* tempItem = mMapScene->asTemporaryItem(clickedItem))
    {
        qDebug() <<  "Clicked TemporaryGraphicsItem:" << static_cast<void*>(tempItem);
    }
}

void vrsa::tools::SingleSelectionTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}

void vrsa::tools::SingleSelectionTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}

void vrsa::tools::SingleSelectionTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{

}

void vrsa::tools::SingleSelectionTool::keyPressEvent(QKeyEvent *event)
{

}

void vrsa::tools::SingleSelectionTool::keyReleaseEvent(QKeyEvent *event)
{

}

void vrsa::tools::SingleSelectionTool::selectVectorFeature(graphics::FeatureGraphicsItem *featureItem)
{
    if (!featureItem)
        return;
    if (mCurrentSelectedFeatureItem)
    {
        mCurrentSelectedFeatureItem->setSelected(false);
        mCurrentSelectedFeatureItem = nullptr;
    }
    featureItem->setSelected(true);
    mCurrentSelectedFeatureItem = featureItem;

}

void vrsa::tools::SingleSelectionTool::deselectCurrentVectorFeature()
{
    if (mCurrentSelectedFeatureItem)
        mCurrentSelectedFeatureItem->setSelected(false);
}
