#include "mapscene.h"
#include <QDebug>
#include "gdal/gdalreader.h"
#include "graphics/featuregraphicsitemfactory.h"

vrsa::graphics::MapScene::MapScene(QObject *parent)
    : QGraphicsScene{parent},
      mMapScale{0.0},
      mMapHolderScale{0.0},
      mFeatures{},
      mCurrentMapTool{nullptr}
{
    setItemIndexMethod(QGraphicsScene::NoIndex);
}

void vrsa::graphics::MapScene::addLayer(std::unique_ptr<vector::VectorLayer> &l)
{

    if (l)
    {
        connect(l.get(), &vector::VectorLayer::featureAdded, this, &graphics::MapScene::onVectorLayerFeatureAdded);
        connect(l.get(), &vector::VectorLayer::featureRemoved, this, &graphics::MapScene::onVectorLayerFeatureRemoved);
    }
    auto pointStyle = VectorFeatureStyle::createDefaultVectorStyle(
        common::GeometryType::Point);
    auto lineStyle = VectorFeatureStyle::createDefaultVectorStyle(
        common::GeometryType::LineString);
    auto polygonStyle = VectorFeatureStyle::createDefaultVectorStyle(
        common::GeometryType::Polygon);


    //todo разделять слои принудительно по типу геометрии для форматов типа geojson poLayer->GetGeomType() = wkbUnknown
    std::unique_ptr<graphics::FeatureGraphicsItem> graphicsItem;
    for (int i=0; i<l->getFeaturesCount(); ++i)
    {
        auto feature = l->getFeatureAt(i);
        switch(feature->getType())
        {
        case common::GeometryType::Point:
        {
            graphicsItem = vrsa::graphics::
                    FeatureGraphicsItemFactory::createForFeature(feature, pointStyle.get());
            qDebug()<<feature->getOGRGeometry()->toPoint()->getX();
            break;
        }
        case common::GeometryType::LineString:
        {
            graphicsItem = vrsa::graphics::
                                FeatureGraphicsItemFactory::createForFeature(feature, lineStyle.get());

            break;
        }

        case common::GeometryType::MultiLineString:
        {
            graphicsItem = vrsa::graphics::
                                FeatureGraphicsItemFactory::createForFeature(feature, lineStyle.get());
            break;
        }
        case common::GeometryType::Polygon:
        {
            graphicsItem = vrsa::graphics::
                                FeatureGraphicsItemFactory::createForFeature(feature, polygonStyle.get());
            break;
        }
        default:
            break;
        }

        addItem(graphicsItem.get());
        graphicsItem->setScale(mMapHolderScale);
        //graphicsItem->setZValue(ag);
        mFeatures.push_back(std::move(graphicsItem));



    }

    l->setStyle(std::move(pointStyle), common::GeometryType::Point);
    l->setStyle(std::move(lineStyle), common::GeometryType::LineString);
    l->setStyle(std::move(polygonStyle), common::GeometryType::Polygon);
    qDebug() << mFeatures.size();
    update();
}

void vrsa::graphics::MapScene::addLayer(std::unique_ptr<raster::RasterChannel> &channel)
{

}

void vrsa::graphics::MapScene::addRasterDataset(raster::RasterDataset *dS)
{
    addItem(RasterGraphicsItemFactory::createForDataset(dS));
}

void vrsa::graphics::MapScene::setTool(std::unique_ptr<tools::MapTool> tool)
{
    if (mCurrentMapTool)
        mCurrentMapTool->cancel();
    mCurrentMapTool = std::move(tool);
    emit toolChanged(mCurrentMapTool.get());
}


void vrsa::graphics::MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit mouseMoved(event->scenePos());
    if (mCurrentMapTool)
        mCurrentMapTool->mouseMoveEvent(event);
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
    QGraphicsScene::mousePressEvent(event);

}

void vrsa::graphics::MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (mCurrentMapTool)
        mCurrentMapTool->mouseReleaseEvent(event);
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
    for (const auto& feat: mFeatures)
    {
        feat->setScale(mMapHolderScale);
    }
    update();
}

void vrsa::graphics::MapScene::onVectorLayerFeatureAdded(vector::VectorFeature *)
{

}

void vrsa::graphics::MapScene::onVectorLayerFeatureRemoved(vector::VectorFeature *)
{

}


