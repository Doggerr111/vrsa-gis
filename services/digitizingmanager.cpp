#include "digitizingmanager.h"

DigitizingManager::DigitizingManager(QObject *parent)
    : QObject{parent}
{

}

void DigitizingManager::onGeometryCreated(vrsa::vector::VectorFeature::GeometryVariant points)
{
    if (!mActiveLayer)
    {
        VRSA_ERROR("DigitizingManager", "Nullptr active layer");
        return;
    }
    switch (mActiveTool->type())
    {
    case vrsa::common::MapToolType::PointDigitizingTool:
    {
        assert(mActiveLayer->getGeomType() == vrsa::common::GeometryType::Point);
//        if (points.size() != 1)
//            throw std::invalid_argument("Point geometry must contain exactly one point");
        
        auto feature = vrsa::vector::VectorFeature::createFeature(mActiveLayer);
        //feature->setGeometry(points[0]);

        
        break;
    }
    default:
        break;
    }
}

void DigitizingManager::onDigitizingFinished()
{

}

void DigitizingManager::onDIgitizingCanceled()
{

}
