#ifndef POINTFEATUREDRAWINGPOLICY_H
#define POINTFEATUREDRAWINGPOLICY_H
#include "graphics/drawing_policies/basepointdrawingpolicy.h"
namespace vrsa
{
namespace graphics
{

class PointFeatureDrawingPolicy: public BasePointDrawingPolicy
{
public:
    PointFeatureDrawingPolicy(const Symbol* symbol);
    // BasePointDrawingPolicy interface
protected:
    void createPath() const override;
    void readGeometry(OGRGeometry* geom) const override;


    // VectorFeatureDrawingPolicy interface
public:
    common::GeometryType getType() const override { return common::GeometryType::Point; };

};


}
}

#endif // POINTFEATUREDRAWINGPOLICY_H
