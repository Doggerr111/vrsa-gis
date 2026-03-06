#include "digitizingmanager.h"
#include "graphics/featuregraphicsitemfactory.h"
DigitizingManager::DigitizingManager(QObject *parent)
    : QObject{parent}
{

}

bool DigitizingManager::isToolCompatibleWithLayer(vrsa::common::MapToolType toolType, vrsa::common::GeometryType layerType) const noexcept
{
    using namespace vrsa::common;
    switch (toolType)
    {
    case MapToolType::PointDigitizingTool:
        return layerType == GeometryType::Point;
    case MapToolType::LineStringDigitizingTool:
        return layerType == GeometryType::LineString ||
                layerType == GeometryType::MultiLineString;
    case MapToolType::PolygonDigitizingTool:
        return layerType == GeometryType::Polygon ||
                layerType == GeometryType::MultiPolygon;
    default:
        return false;
    }
}

void DigitizingManager::onGeometryCreated(vrsa::geometry::Geometry &geometry)
{
    if (!mActiveLayer)
    {
        VRSA_ERROR("DigitizingManager", "Select active layer to start digitizing");
        return;
    }

    if (!isToolCompatibleWithLayer(mActiveTool->type(), mActiveLayer->getGeomType()))
    {
        VRSA_ERROR("DigitizingManager", "Active Tool has wrong type");
        return;
    }

    auto feature = vrsa::vector::VectorFeature::createFeature(mActiveLayer);
    if (!feature)
        return;
    if (!feature->setGeometry(geometry))
        return;
    auto item = vrsa::graphics::FeatureGraphicsItemFactory::createForFeature(feature.get(),
                                                                 mActiveLayer->getStyle(mActiveLayer->getGeomType())); //hmmm
    if (!item)
        return;
    connect(mActiveLayer, &vrsa::vector::VectorLayer::ZValueChanged, item.get(),
            &vrsa::graphics::FeatureGraphicsItem::onZValueChanged);
    if (mActiveLayer->addFeature(std::move(feature)))
    {
        emit featureGraphicsItemCreated(item);
        return;
    }
    else
    {
        item.reset();
        return;
    }
}
//    switch (mActiveTool->type())
//    {
//    case vrsa::common::MapToolType::PointDigitizingTool:
//    {
//        assert(mActiveLayer->getGeomType() == vrsa::common::GeometryType::Point);
//        auto feature = vrsa::vector::VectorFeature::createFeature(mActiveLayer);
//        if (!feature)
//            return;
//        feature->setGeometry(geometry);
//        auto pointItem = vrsa::graphics::FeatureGraphicsItemFactory::createForFeature(feature.get(),
//                                                                     mActiveLayer->getStyle(vrsa::common::GeometryType::Point));
//        if (!pointItem)
//            return;

//        connect(mActiveLayer, &vrsa::vector::VectorLayer::ZValueChanged, pointItem.get(),
//                &vrsa::graphics::FeatureGraphicsItem::onZValueChanged);
//        mActiveLayer->addFeature(std::move(feature));
//        emit featureGraphicsItemCreated(pointItem);
//        break;
//    }
//    case vrsa::common::MapToolType::LineStringDigitizingTool:
//    {
//        assert(mActiveLayer->getGeomType() == vrsa::common::GeometryType::LineString);
//        auto feature = vrsa::vector::VectorFeature::createFeature(mActiveLayer);
//        if (!feature)
//            return;
//        if (!feature->setGeometry(geometry))
//            return;
//        auto lineItem = vrsa::graphics::FeatureGraphicsItemFactory::createForFeature(feature.get(),
//                                                                     mActiveLayer->getStyle(vrsa::common::GeometryType::LineString));
//        if (!lineItem)
//            return;
//        connect(mActiveLayer, &vrsa::vector::VectorLayer::ZValueChanged, lineItem.get(),
//                &vrsa::graphics::FeatureGraphicsItem::onZValueChanged);
//        if (mActiveLayer->addFeature(std::move(feature)))
//        {
//            emit featureGraphicsItemCreated(lineItem);
//            return;
//        }
//        else
//        {
//            lineItem.reset();
//            return;
//        }
//    }
//    default:
//        break;
//    }


void DigitizingManager::onDigitizingFinished()
{
    mActiveLayer = nullptr;
    mActiveTool = nullptr;
}

void DigitizingManager::onDigitizingCanceled()
{

}
