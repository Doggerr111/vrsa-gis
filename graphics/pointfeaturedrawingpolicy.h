#ifndef POINTFEATUREDRAWINGPOLICY_H
#define POINTFEATUREDRAWINGPOLICY_H
#include "graphics/vectorfeaturedrawingpolicy.h"
#include <QPainterPath>
namespace vrsa
{
namespace graphics
{

class SimplePointSymbol;

class PointFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{

public:
    PointFeatureDrawingPolicy(const Symbol *symbol);

    // VectorFeatureDrawingPolicy interface
public:
    void cacheGeometry(OGRGeometry* geom) const override;
    void paint(const DrawingContext& context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext& context) const override;
    void rebuildCache(const DrawingContext &context) override;
    //QPainterPath geometryToPath(const DrawingContext &context) const override;
private:
    mutable double mX, mY;
    bool isGeomValid = false;
    double mPointSize;
    const SimplePointSymbol* mSymbol;

};

class MultiPointFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    MultiPointFeatureDrawingPolicy(const Symbol *symbol);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext& context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    void cacheGeometry(OGRGeometry* geom) const override;

private:
    mutable std::vector<QPointF> mPoints;
    const SimplePointSymbol* mSymbol;
};

}
}

#endif // POINTFEATUREDRAWINGPOLICY_H
