#ifndef POLYGONFEATUREDRAWINGPOLICY_H
#define POLYGONFEATUREDRAWINGPOLICY_H
#include "graphics/drawing_policies/basepolygondrawingpolicy.h"

namespace vrsa
{
namespace graphics
{

class PolygonFeatureDrawingPolicy: public BasePolygonDrawingPolicy
{
public:
    PolygonFeatureDrawingPolicy(const Symbol* symbol);
    // BasePointDrawingPolicy interface
protected:
    QPainterPath createPathFromOGRGeometry(OGRGeometry *geom) const override;

    // VectorFeatureDrawingPolicy interface
public:
    inline common::GeometryType getType() const override { return common::GeometryType::Polygon; };
};

}
}

#endif // POLYGONFEATUREDRAWINGPOLICY_H
