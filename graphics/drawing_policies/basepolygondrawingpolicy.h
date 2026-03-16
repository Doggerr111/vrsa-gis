#ifndef BASEPOLYGONDRAWINGPOLICY_H
#define BASEPOLYGONDRAWINGPOLICY_H
#include "graphics/drawing_policies/vectorfeaturedrawingpolicy.h"

namespace vrsa
{
namespace graphics
{
class SimplePolygonSymbol;
class BasePolygonDrawingPolicy : public VectorFeatureDrawingPolicy
{
public:
    BasePolygonDrawingPolicy(const Symbol* symbol);
public:
    void paint         (const DrawingContext &context) override;
    QRectF boundingRect(const DrawingContext &context) const override;
    inline common::GeometryType getType()        const override {return vrsa::common::GeometryType::Unknown;};
protected:
    //шаблонный метод - линия и мультилиния реализуют по-своему
    virtual QPainterPath createPathFromOGRGeometry(OGRGeometry* geom) const = 0;
private:
    void cacheGeometry (OGRGeometry *geom, double scale)       const override
    {
        if (!mCache.isScaleValid(scale))
            updateScaleDependentParams(scale);
            mCache.path = createPathFromOGRGeometry(geom);
            mCache.isGeomValid = true;
            updateStrokePath();
    };
    void updateScaleDependentParams(double scale)const override;
    void updateStrokePath()                      const override;
protected:
    const SimplePolygonSymbol* mSymbol;
};

}
}

#endif // BASEPOLYGONDRAWINGPOLICY_H
