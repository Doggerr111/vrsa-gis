#ifndef MULTIPOINTFEATUREDRAWINGPOLICY_H
#define MULTIPOINTFEATUREDRAWINGPOLICY_H
#include "graphics/drawing_policies/basepointdrawingpolicy.h"

namespace vrsa
{
namespace graphics
{

class MultiPointFeatureDrawingPolicy: public BasePointDrawingPolicy
{
public:
    MultiPointFeatureDrawingPolicy(const Symbol* symbol);
    // BasePointDrawingPolicy interface
protected:
    virtual void readGeometry(OGRGeometry* geom) const override;
    virtual void createPath() const override;
    //QPainterPath createPathFromOGRGeometry(OGRGeometry *geom) const override;

    // VectorFeatureDrawingPolicy interface
public:
    inline common::GeometryType getType() const override { return common::GeometryType::Point; };
};

}
}

#endif // MULTIPOINTFEATUREDRAWINGPOLICY_H
