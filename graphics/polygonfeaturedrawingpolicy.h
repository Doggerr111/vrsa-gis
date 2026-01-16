#ifndef POLYGONFEATUREDRAWINGPOLICY_H
#define POLYGONFEATUREDRAWINGPOLICY_H
#include "graphics/vectorfeaturedrawingpolicy.h"

namespace vrsa
{
namespace graphics
{


class PolygonFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    PolygonFeatureDrawingPolicy(VectorFeatureStyle &style);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    void cacheGeometry(OGRGeometry *geom) override;
private:
    QPainterPath path;
};


class MultiPolygonFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    MultiPolygonFeatureDrawingPolicy(VectorFeatureStyle &style);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    void cacheGeometry(OGRGeometry *geom) override;
private:
    QPainterPath path;
};

}
}

#endif // POLYGONFEATUREDRAWINGPOLICY_H
