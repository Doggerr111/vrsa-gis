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

void vrsa::graphics::MapScene::addLayer(std::unique_ptr<vector::VectorLayer> &l)
{

//    if (l)
//    {
//        connect(l.get(), &vector::VectorLayer::featureAdded, this, &graphics::MapScene::onVectorLayerFeatureAdded);
//        connect(l.get(), &vector::VectorLayer::featureRemoved, this, &graphics::MapScene::onVectorLayerFeatureRemoved);
//    }
//    auto pointStyle = VectorFeatureStyle::createDefaultVectorStyle(
//                common::GeometryType::Point);
//    auto lineStyle = VectorFeatureStyle::createDefaultVectorStyle(
//                common::GeometryType::LineString);
//    auto polygonStyle = VectorFeatureStyle::createDefaultVectorStyle(
//                common::GeometryType::Polygon);


//    //todo разделять слои принудительно по типу геометрии для форматов типа geojson poLayer->GetGeomType() = wkbUnknown
//    std::unique_ptr<graphics::FeatureGraphicsItem> graphicsItem;
//    for (int i=0; i<l->getFeaturesCount(); ++i)
//    {
//        auto feature = l->getFeatureAt(i);
//        switch(feature->getType())
//        {
//        case common::GeometryType::Point:
//        {
//            graphicsItem = vrsa::graphics::
//                    FeatureGraphicsItemFactory::createForFeature(feature, pointStyle.get());
//            //qDebug()<<feature->getOGRGeometry()->toPoint()->getX();
//            break;
//        }
//        case common::GeometryType::LineString:
//        {
//            graphicsItem = vrsa::graphics::
//                    FeatureGraphicsItemFactory::createForFeature(feature, lineStyle.get());

//            break;
//        }

//        case common::GeometryType::MultiLineString:
//        {
//            graphicsItem = vrsa::graphics::
//                    FeatureGraphicsItemFactory::createForFeature(feature, lineStyle.get());
//            break;
//        }
//        case common::GeometryType::Polygon:
//        {
//            graphicsItem = vrsa::graphics::
//                    FeatureGraphicsItemFactory::createForFeature(feature, polygonStyle.get());
//            break;
//        }
//        default:
//            break;
//        }

//        if (graphicsItem)
//        {
//            addItem(graphicsItem.get());
//            graphicsItem->setScale(mMapHolderScale);
//            connect(l.get(), &vector::VectorLayer::ZValueChanged, graphicsItem.get(), &FeatureGraphicsItem::onZValueChanged);
//            mFeatures.push_back(std::move(graphicsItem));
//        }
//    }

//    l->setStyle(std::move(pointStyle), common::GeometryType::Point);
//    l->setStyle(std::move(lineStyle), common::GeometryType::LineString);
//    l->setStyle(std::move(polygonStyle), common::GeometryType::Polygon);
//    qDebug() << mFeatures.size();
//    update();
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
    mMapScale = mapScale;
    mMapHolderScale = widgetScale;
    for (const auto& feat: mFeatures) //для векторных объектов
    {
        feat->setScale(mMapHolderScale);
    }
    for (const auto& tempItem: mTempItems) //для временных объектов
    {
        tempItem->setScale(mMapHolderScale);
    }
    //update();
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
    item->setScale(mMapHolderScale);
    addItem(item.get());
    mFeatures.push_back(std::move(item));
    //item->update();
    update();
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
    item->setScale(mMapHolderScale);
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


