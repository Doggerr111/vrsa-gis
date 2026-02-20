#ifndef LINEFEATUREDRAWINGPOLICY_H
#define LINEFEATUREDRAWINGPOLICY_H
#include <vectorfeaturedrawingpolicy.h>
namespace vrsa
{
namespace graphics
{

class SimpleLineSymbol;

class LineFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    LineFeatureDrawingPolicy(const Symbol* symbol);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    //QPainterPath geometryToPath(const DrawingContext &context) const override;
    void cacheGeometry(OGRGeometry *geom) const override;
private:
    //std::vector<QPointF> mPoints;
    const SimpleLineSymbol* mSymbol;
};

class MultiLineFeatureDrawingPolicy: public VectorFeatureDrawingPolicy
{
public:
    MultiLineFeatureDrawingPolicy(const Symbol* symbol);

    // VectorFeatureDrawingPolicy interface
public:
    void paint(const DrawingContext &context) override;
    common::GeometryType getType() const override;
    QRectF boundingRect(const DrawingContext &context) const override;
    void rebuildCache(const DrawingContext &context) override;
    //QPainterPath geometryToPath(const DrawingContext &context) const override;
    void cacheGeometry(OGRGeometry *geom) const override;
private:
    const SimpleLineSymbol* mSymbol;
};


}
}

#endif // LINEFEATUREDRAWINGPOLICY_H
