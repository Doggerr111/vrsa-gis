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
    inline common::GeometryType getType() const override {return vrsa::common::GeometryType::Unknown;};
    void updateSymbol(const Symbol* symbol) override;
protected:
    //шаблонный метод - полигон и мультиполигон реализовывают по разному
    QPainterPath createPathFromOGRGeometry(OGRGeometry* geom) const override{ return {}; };
private:
    void cacheGeometry (const DrawingContext &context)      const override
    {
        if (!mCache.isScaleValid(context.sceneScale))
            updateScaleDependentParams(context.sceneScale);
        if (context.isLodOn)
            precomputeLOD(context.geom);
        else
        {
            mCache.isGeomValid = true;
            mCache.path = createPathFromOGRGeometry(context.geom);
            mCache.geom = context.geom;
        }
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
