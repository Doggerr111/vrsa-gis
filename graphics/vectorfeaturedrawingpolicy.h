#ifndef FEATUREDRAWINGPOLICY_H
#define FEATUREDRAWINGPOLICY_H
#include <QPainter>
#include <ogr_geometry.h>
#include <QStyleOptionGraphicsItem>
#include "common/GisDefines.h"
#include "graphics/drawingcontext.h"
#include "graphics/vectorfeaturestyle.h"
namespace vrsa
{
namespace graphics
{


class VectorFeatureDrawingPolicy
{
public:
    VectorFeatureDrawingPolicy(VectorFeatureStyle &style);
    virtual void paint(const DrawingContext& context) const = 0;
    virtual vrsa::common::GeometryType getType() const = 0;
    virtual ~VectorFeatureDrawingPolicy() = default;
    virtual QRectF boundingRect() const = 0;
protected:
    VectorFeatureStyle mStyle;
};
}
}
#endif // FEATUREDRAWINGPOLICY_H
