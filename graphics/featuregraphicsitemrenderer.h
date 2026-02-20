#ifndef GRAPHICSITEMRENDERER_H
#define GRAPHICSITEMRENDERER_H

#include "graphics/vectorfeaturestyle.h"
#include "graphics/drawingcontext.h"
#include "graphics/vectorfeaturedrawingpolicy.h"
namespace vrsa
{
namespace graphics
{


class FeatureGraphicsItemRenderer
{
public:
    FeatureGraphicsItemRenderer(VectorFeatureStyle* style, common::GeometryType featureType);
    void paint(const DrawingContext& context);
    QRectF boundingRect(const DrawingContext& context);
    void renderSymbol(Symbol* symbol, const DrawingContext& context);
    void createPolicy(Symbol* symbol);

private:
    VectorFeatureStyle* mStyle;
    common::GeometryType mFeatureType;
    std::vector<std::unique_ptr<VectorFeatureDrawingPolicy>> mPolicies;
};

}
}

#endif // GRAPHICSITEMRENDERER_H
