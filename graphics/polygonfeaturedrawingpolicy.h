#ifndef POLYGONFEATUREDRAWINGPOLICY_H
#define POLYGONFEATUREDRAWINGPOLICY_H
#include "graphics/vectorfeaturedrawingpolicy.h"
#include "simplepolygonsymbol.h"
namespace vrsa
{
namespace graphics
{


class PolygonFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    PolygonFeatureDrawingPolicy(Symbol* style);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    void cacheGeometry(OGRGeometry *geom) const override;
private:
    QPainterPath path;
    const SimplePolygonSymbol* mSymbol;
};


class MultiPolygonFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    MultiPolygonFeatureDrawingPolicy(Symbol* style);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    void cacheGeometry(OGRGeometry *geom) const override;
private:
    QPainterPath path;
    const SimplePolygonSymbol* mSymbol;
};

}
}

#endif // POLYGONFEATUREDRAWINGPOLICY_H
