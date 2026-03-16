#ifndef BASEPOINTDRAWINGPOLICY_H
#define BASEPOINTDRAWINGPOLICY_H
#include "graphics/drawing_policies/vectorfeaturedrawingpolicy.h"

namespace vrsa
{
namespace graphics
{
class SimplePointSymbol;
class BasePointDrawingPolicy : public VectorFeatureDrawingPolicy
{
public:
    BasePointDrawingPolicy(const Symbol* symbol);
public:
    void paint         (const DrawingContext &context) override;
    QRectF boundingRect(const DrawingContext &context) const override;
    inline common::GeometryType getType()        const override {return vrsa::common::GeometryType::Unknown;};
protected:
    virtual void readGeometry(OGRGeometry* geom) const = 0;
    virtual void createPath() const = 0;
private:
    //Так как точка - единственный тип геометрии, путь которой зависит от масштаба, то тут алгоритм другой:
    //сначала кешируем mX, mY, а в методе updateScaleDependentParams рассчитываем path
    void cacheGeometry (OGRGeometry *geom, double scale)       const override
    {
        readGeometry(geom);
        if (!mCache.isScaleValid(scale))
            updateScaleDependentParams(scale); //вот тут будем обновлять путь методом cachePath()
        mCache.isGeomValid = true;
        //updateStrokePath();
    };
    void updateScaleDependentParams(double scale)const override;
    void updateStrokePath()                      const override;
protected:
    const SimplePointSymbol* mSymbol;
    mutable double mX, mY;
    mutable std::vector<QPointF> mPoints; //i hate myself
};

}
}

#endif // BASEPOINTDRAWINGPOLICY_H
