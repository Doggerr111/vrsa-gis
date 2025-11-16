#ifndef POINTFEATUREDRAWINGPOLICY_H
#define POINTFEATUREDRAWINGPOLICY_H
#include "graphics/vectorfeaturedrawingpolicy.h"
#include <QPainterPath>
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
    void cacheGeometry(OGRGeometry* geom) override;
    void paint(const DrawingContext& context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext& context) const override;
    void rebuildCache(const DrawingContext &context) override;
    //QPainterPath geometryToPath(const DrawingContext &context) const override;
private:
    double mX, mY;
    bool isGeomValid = false;
    double mPointSize;

};

class MultiPointFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    MultiPointFeatureDrawingPolicy(VectorFeatureStyle &style);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext& context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    void cacheGeometry(OGRGeometry* geom) override;

private:
    std::vector<QPointF> mPoints;
};

}
}

#endif // POINTFEATUREDRAWINGPOLICY_H
