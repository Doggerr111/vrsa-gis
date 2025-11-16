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
    LineFeatureDrawingPolicy(VectorFeatureStyle &style);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    //QPainterPath geometryToPath(const DrawingContext &context) const override;
    void cacheGeometry(OGRGeometry *geom) override;
private:
    //std::vector<QPointF> mPoints;
    QPainterPath path;
};

class MultiLineFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    MultiLineFeatureDrawingPolicy(VectorFeatureStyle& style);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    //QPainterPath geometryToPath(const DrawingContext &context) const override;
    void cacheGeometry(OGRGeometry *geom) override;
};


}
}

#endif // LINEFEATUREDRAWINGPOLICY_H
