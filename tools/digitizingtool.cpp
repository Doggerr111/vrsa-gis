#include "digitizingtool.h"

#include "vector/vectorlayer.h"
#include "tools/pointdigitizingtool.h"
#include "tools/linedigitizingtool.h"
#include "graphics/featuregraphicsitem.h"
#include "graphics/featuregraphicsitemfactory.h"
#include "graphics/mapscene.h"

vrsa::tools::DigitizingTool::DigitizingTool(graphics::MapScene* scene, vector::VectorLayer* activeLayer, QObject *parent)
    : MapTool{scene, parent},
      mDigitizeManager{std::make_unique<DigitizeHelper>(parent)}
{
    if (mDigitizeManager && activeLayer)
    {
    mDigitizeManager->setActiveLayer(activeLayer);
    connect(this, &tools::MapTool::cancelled, mDigitizeManager.get(),
            &DigitizeHelper::onDigitizingCanceled);
    connect(this, &tools::DigitizingTool::geometryCreated, mDigitizeManager.get(),
            &DigitizeHelper::onGeometryCreated);
    connect(this, &tools::MapTool::deactivated, mDigitizeManager.get(),
            &DigitizeHelper::onDigitizingFinished);
    mDigitizeManager->setActiveDigitizingTool(this);
    connect(mDigitizeManager.get(), &DigitizeHelper::featureGraphicsItemCreated, mMapScene,
            &vrsa::graphics::MapScene::onNewFeatureGraphicsItemCreated);
    }
}



vrsa::tools::DigitizeHelper::DigitizeHelper::DigitizeHelper(QObject *parent)
    : QObject{parent}
{

}

bool vrsa::tools::DigitizeHelper::DigitizeHelper::isToolCompatibleWithLayer(vrsa::common::MapToolType toolType, vrsa::common::GeometryType layerType) const noexcept
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

void vrsa::tools::DigitizeHelper::DigitizeHelper::onGeometryCreated(vrsa::geometry::Geometry &geometry)
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


//    connect(mActiveLayer, &vrsa::vector::VectorLayer::ZValueChanged, item.get(),
//            &vrsa::graphics::FeatureGraphicsItem::onZValueChanged);
    auto featRawPtr = feature.get();
    if (mActiveLayer->addFeature(std::move(feature)))
    {
        auto item = graphics::FeatureGraphicsItemFactory::instance().createForFeature(featRawPtr);
//        auto item = std::make_unique<graphics::FeatureGraphicsItem>(feature.get());

//        emit featureGraphicsItemCreated(item);
        return;
    }
    else
        return;
}



void vrsa::tools::DigitizeHelper::DigitizeHelper::onDigitizingFinished()
{
    mActiveLayer = nullptr;
    mActiveTool = nullptr;
}

void vrsa::tools::DigitizeHelper::DigitizeHelper::onDigitizingCanceled()
{

}

