#include "mapscene.h"
#include <QDebug>
#include "gdal/gdalreader.h"
#include "graphics/featuregraphicsitemfactory.h"
#include <QGraphicsView>
#include <QMessageBox>

vrsa::graphics::MapScene::MapScene(QObject *parent)
    : QGraphicsScene{parent},
      mMapScale{0.0},
      mMapHolderScale{0.0},
      mFeatures{},
      mCurrentMapTool{nullptr}
{
    //устанавливаем sceneRect в координатах EPSG:3857
    //множитель 4 и -8 для увеличения рабочей области
    setSceneRect(X_MIN * 4, 4 * Y_MAX, 4 * WIDTH, -8 * HEIGHT);
    setItemIndexMethod(QGraphicsScene::NoIndex);
}



void vrsa::graphics::MapScene::addLayer(std::unique_ptr<raster::RasterChannel> &channel)
{

}

void vrsa::graphics::MapScene::addRasterDataset(raster::RasterDataset *dS)
{
    addItem(RasterGraphicsItemFactory::createForDataset(dS));
}

void vrsa::graphics::MapScene::addTemporaryItem(std::unique_ptr<TemporaryGraphicsItem> item)
{
    if (!item)
        return;
    item->setScale(mMapHolderScale);
    qDebug()<<"adding temp item";
    addItem(item.get());
    auto itemf = item.get();
    //qDebug()<<"item added";
    //qDebug()<<item->scene();
    mTempItems.push_back(std::move(item));
    //qDebug()<<itemf->scene();
    //update();
}

void vrsa::graphics::MapScene::removeTemporaryItems()
{
    qDebug()<< "we're about to remove temp items..";
    mTempItems.clear();
}

void vrsa::graphics::MapScene::setMapTool(std::unique_ptr<tools::MapTool> tool)
{
    if (mCurrentMapTool)
        mCurrentMapTool->cancel();
    //QMessageBox::warning(nullptr, "we still save !", "we still save yey!!!");
    mCurrentMapTool = std::move(tool);
    setViewCursor(mCurrentMapTool->cursor());
    emit toolChanged(mCurrentMapTool.get());
}

void vrsa::graphics::MapScene::deselectCurrentMapTool()
{
    //QMessageBox::warning(nullptr, "WTF!!", "WTF!!");
    if (mCurrentMapTool)
    {
        setViewCursor(Qt::ArrowCursor);
        mCurrentMapTool->deactivate();
        mCurrentMapTool.reset();
        mCurrentMapTool = nullptr;
        removeTemporaryItems();
    }

}

void vrsa::graphics::MapScene::setViewCursor(QCursor cursor)
{
    for (QGraphicsView* view : views()) {
            view->setCursor(cursor);
        }
}


void vrsa::graphics::MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit mouseMoved(event->scenePos());
    if (mCurrentMapTool)
        mCurrentMapTool->mouseMoveEvent(event);

    if (event->isAccepted())
        return;
    QGraphicsScene::mouseMoveEvent(event);
}

void vrsa::graphics::MapScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (mCurrentMapTool)
        mCurrentMapTool->wheelEvent(event);
    QGraphicsScene::wheelEvent(event);
}

void vrsa::graphics::MapScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    if (mCurrentMapTool)
        mCurrentMapTool->mousePressEvent(event);

    if (event->isAccepted())
        return;

    QGraphicsScene::mousePressEvent(event);

}

void vrsa::graphics::MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (mCurrentMapTool)
        mCurrentMapTool->mouseReleaseEvent(event);
    if (event->isAccepted()) {
                return;
            }
    QGraphicsScene::mouseReleaseEvent(event);
}

void vrsa::graphics::MapScene::keyPressEvent(QKeyEvent *event)
{
    if (mCurrentMapTool)
        mCurrentMapTool->keyPressEvent(event);
    QGraphicsScene::keyPressEvent(event);

}

void vrsa::graphics::MapScene::onMapHolderScaleChanged(int mapScale, double widgetScale)
{
    //qDebug()<<"new scale:" << mapScale;
    mMapScale = mapScale;
    mMapHolderScale = widgetScale;
    for (const auto& feat: mFeatures) //для векторных объектов
    {
        feat->setScale(mMapScale, mMapHolderScale);
    }
    for (const auto& tempItem: mTempItems) //для временных объектов
    {
        tempItem->setScale(mMapHolderScale);
    }

}

void vrsa::graphics::MapScene::onVectorLayerFeatureAdded(vector::VectorFeature *)
{

}

void vrsa::graphics::MapScene::onVectorLayerFeatureRemoved(int64_t fid)
{

}

//delete this
void vrsa::graphics::MapScene::onNewFeatureGraphicsItemCreated(std::unique_ptr<FeatureGraphicsItem> &item)
{
    item->setScale(mMapScale, mMapHolderScale);
    addItem(item.get());
    mFeatures.push_back(std::move(item));
    //item->update();
    update(item->boundingRect());
}

//from proj manager::featureGraphicsItemCreated
void vrsa::graphics::MapScene::onFeatureGraphicsItemCreated(FeatureGraphicsItem *item)
{
    if (!item)
    {
        VRSA_WARNING("RENDERING", "Failed to add Feature Graphics Item (nullptr) to scene");
        return;
    }
    addItem(item);
    item->setScale(mMapScale, mMapHolderScale);
    mFeatures.push_back(std::unique_ptr<FeatureGraphicsItem>(item));
    update(item->boundingRect());
    qDebug()<<items().count();
}

//void vrsa::graphics::MapScene::onFeatureGraphicsItemCreated(std::unique_ptr<FeatureGraphicsItem> item)
//{
//    addItem(item.get());
//    mFeatures.push_back(std::move(item));
//    update(item->boundingRect());
//}

void vrsa::graphics::MapScene::onMapHolderMousePressed(QMouseEvent *event)
{
    emit panningRequested(mPanningForViewEnabled);



}


