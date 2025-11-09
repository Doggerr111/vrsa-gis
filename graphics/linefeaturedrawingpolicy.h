#ifndef LINEFEATUREDRAWINGPOLICY_H
#define LINEFEATUREDRAWINGPOLICY_H
#include <vectorfeaturedrawingpolicy.h>
namespace vrsa
{
namespace graphics
{

class LineFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    LineFeatureDrawingPolicy();

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) const override;
    common::GeometryType getType() const override;
};

class MultiLineFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    MultiLineFeatureDrawingPolicy();

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) const override;
    common::GeometryType getType() const override;
};


}
}

#endif // LINEFEATUREDRAWINGPOLICY_H
