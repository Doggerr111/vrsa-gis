#ifndef POINTFEATUREDRAWINGPOLICY_H
#define POINTFEATUREDRAWINGPOLICY_H
#include "graphics/vectorfeaturedrawingpolicy.h"
namespace vrsa
{
namespace graphics
{


class PointFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    PointFeatureDrawingPolicy(VectorFeatureStyle &style);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext& context) const override;
    common::GeometryType getType() const override;
    QRectF boundingRect() const override;
};

class MultiPointFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    MultiPointFeatureDrawingPolicy();

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext& context) const;
    common::GeometryType getType() const;
};

}
}

#endif // POINTFEATUREDRAWINGPOLICY_H
