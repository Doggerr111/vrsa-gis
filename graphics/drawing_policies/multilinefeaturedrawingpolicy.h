#ifndef MULTILINEFEATUREDRAWINGPOLICY_H
#define MULTILINEFEATUREDRAWINGPOLICY_H
#include "graphics/drawing_policies/baselinedrawingpolicy.h"

namespace vrsa
{
namespace graphics
{

class MultiLineFeatureDrawingPolicy: public BaseLineDrawingPolicy
{
public:
    MultiLineFeatureDrawingPolicy(const Symbol* symbol);
    // BaseLineDrawingPolicy interface
protected:
    QPainterPath createPathFromOGRGeometry(OGRGeometry *geom) const override;

    // VectorFeatureDrawingPolicy interface
public:
    inline common::GeometryType getType() const override {return vrsa::common::GeometryType::MultiLineString;};

};

}
}

#endif // MULTILINEFEATUREDRAWINGPOLICY_H
