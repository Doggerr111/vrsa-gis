#ifndef BASELINEDRAWINGPOLICY_H
#define BASELINEDRAWINGPOLICY_H
#include "graphics/drawing_policies/vectorfeaturedrawingpolicy.h"

namespace vrsa
{
namespace graphics
{
class SimpleLineSymbol;
class BaseLineDrawingPolicy : public VectorFeatureDrawingPolicy
{
public:
    BaseLineDrawingPolicy(const Symbol* symbol);
public:
    void paint         (const DrawingContext &context) override;
    QRectF boundingRect(const DrawingContext &context) const override;
    inline common::GeometryType getType()        const override {return vrsa::common::GeometryType::Unknown;};
    void updateSymbol(const Symbol* symbol) override;
protected:
    //шаблонный метод - линия и мультилиния реализуют по-своему
    QPainterPath createPathFromOGRGeometry(OGRGeometry* geom) const override{ return {}; };
private:
    void cacheGeometry (const DrawingContext &context) const override
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
private:
    const SimpleLineSymbol* mSymbol;
};

}
}

#endif // BASELINEDRAWINGPOLICY_H
