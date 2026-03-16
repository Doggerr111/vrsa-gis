#ifndef MULTIPOLYGONFEATUREDRAWINGPOLICY_H
#define MULTIPOLYGONFEATUREDRAWINGPOLICY_H
#include "graphics/drawing_policies/basepolygondrawingpolicy.h"

namespace vrsa
{
namespace graphics
{


class MultiPolygonFeatureDrawingPolicy: public BasePolygonDrawingPolicy
{
public:
    MultiPolygonFeatureDrawingPolicy(const Symbol* symbol);
    // BasePointDrawingPolicy interface
protected:
    QPainterPath createPathFromOGRGeometry(OGRGeometry *geom) const override;

    // VectorFeatureDrawingPolicy interface
public:
    inline common::GeometryType getType() const override { return common::GeometryType::MultiPolygon; };
};
}
}

#endif // MULTIPOLYGONFEATUREDRAWINGPOLICY_H
