#ifndef FEATUREGRAPHICSITEMFACTORY_H
#define FEATUREGRAPHICSITEMFACTORY_H

#include "graphics/featuregraphicsitem.h"
//#include "graphics/vectorfeaturedrawingpolicy.h"
#include "graphics/pointfeaturedrawingpolicy.h"
#include "graphics/linefeaturedrawingpolicy.h"
#include "vector/vectorfeature.h"
#include "graphics/vectorfeaturestyle.h"
#include "graphics/polygonfeaturedrawingpolicy.h"

namespace vrsa
{
namespace graphics
{

class FeatureGraphicsItemFactory
{
public:
    FeatureGraphicsItemFactory();

public:
    static std::unique_ptr<FeatureGraphicsItem> createForFeature(
            const vrsa::vector::VectorFeature* feature, vrsa::graphics::VectorFeatureStyle style)
    {
        auto strategy = createPainterPolicy(feature, style);
        auto item = std::make_unique<FeatureGraphicsItem>(std::move(strategy), feature);
        QObject::connect(feature, &vector::VectorFeature::visibilityChanged,
                         item.get(), &FeatureGraphicsItem::setVisible);
        return item;
    }


    //static std::unique_ptr
private:
    static std::unique_ptr<VectorFeatureDrawingPolicy> createPainterPolicy(
            const vrsa::vector::VectorFeature* feature,
            VectorFeatureStyle& style
        ) {
            using namespace vrsa::common;
            switch (feature->getType()) {
            case GeometryType::Point:
                return std::make_unique<PointFeatureDrawingPolicy>(style);
            case GeometryType::MultiPoint:
                return std::make_unique<MultiPointFeatureDrawingPolicy>(style);
            case GeometryType::LineString:
                return std::make_unique<LineFeatureDrawingPolicy>(style);
            case GeometryType::MultiLineString:
                return std::make_unique<MultiLineFeatureDrawingPolicy>(style);
            case GeometryType::Polygon:
                return std::make_unique<PolygonFeatureDrawingPolicy>(style);
            case GeometryType::MultiPolygon:
                return std::make_unique<MultiPolygonFeatureDrawingPolicy>(style);
            }
            //return std::make_unique<UnknownGeometryPainterStrategy>(style);
        }
};
}
}

#endif // FEATUREGRAPHICSITEMFACTORY_H
