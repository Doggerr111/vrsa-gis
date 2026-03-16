#ifndef LINEFEATUREDRAWINGPOLICY_H
#define LINEFEATUREDRAWINGPOLICY_H
#include "graphics/drawing_policies/baselinedrawingpolicy.h"

namespace vrsa
{
namespace graphics
{

class SimpleLineSymbol;

class LineFeatureDrawingPolicy: public BaseLineDrawingPolicy
{
public:
    LineFeatureDrawingPolicy(const Symbol* symbol);
    // BaseLineDrawingPolicy interface
protected:
    QPainterPath createPathFromOGRGeometry(OGRGeometry *geom) const override;

    // VectorFeatureDrawingPolicy interface
public:
    inline common::GeometryType getType() const override { return common::GeometryType::LineString; };
};


}
}

#endif // LINEFEATUREDRAWINGPOLICY_H
