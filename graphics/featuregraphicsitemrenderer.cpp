#include "featuregraphicsitemrenderer.h"
#include "pointfeaturedrawingpolicy.h"
#include "linefeaturedrawingpolicy.h"
#include "polygonfeaturedrawingpolicy.h"


vrsa::graphics::FeatureGraphicsItemRenderer::FeatureGraphicsItemRenderer(VectorFeatureStyle *style,
                                                                         common::GeometryType type)
    : mStyle{style},
      mFeatureType{type}
{
    createPolicy(mStyle->getSymbol());
}

void vrsa::graphics::FeatureGraphicsItemRenderer::paint(const DrawingContext &context)
{
    for (const auto& policy: mPolicies)
        policy->paint(context);
}

QRectF vrsa::graphics::FeatureGraphicsItemRenderer::boundingRect(const DrawingContext &context)
{
    QRectF totalBoundingRect;
    bool first = true;
    for (const auto& policy: mPolicies)
    {
        if (first)
        {
            totalBoundingRect = policy->boundingRect(context);
            first = false;
        }
        else
            totalBoundingRect = totalBoundingRect.united(policy->boundingRect(context));
    }
    return totalBoundingRect;
}

void vrsa::graphics::FeatureGraphicsItemRenderer::renderSymbol(Symbol *symbol, const DrawingContext &context)
{

}

void vrsa::graphics::FeatureGraphicsItemRenderer::createPolicy(Symbol *symbol)
{
    if (!symbol)
        return;
    if (symbol->canHaveChildren())
    {
        for (int i = 0; i < symbol->childCount(); ++i)
        {
            createPolicy(symbol->child(i));
        }
        return;
    }
    switch (symbol->type())
    {
    case common::SymbolType::SimplePointSymbol:
    {
        if (mFeatureType == common::GeometryType::Point)
            mPolicies.push_back(std::make_unique<PointFeatureDrawingPolicy>(symbol));
        else if (mFeatureType == common::GeometryType::MultiPoint)
            mPolicies.push_back(std::make_unique<MultiPointFeatureDrawingPolicy>(symbol));
        break;

    }
    case common::SymbolType::SimpleLineSymbol:
    {
        if (mFeatureType == common::GeometryType::LineString)
            mPolicies.push_back(std::make_unique<LineFeatureDrawingPolicy>(symbol));
        else if (mFeatureType == common::GeometryType::MultiLineString)
            mPolicies.push_back(std::make_unique<MultiLineFeatureDrawingPolicy>(symbol));
        break;
    }
    case common::SymbolType::SimplePolygonSymbol:
    {
        if (mFeatureType == common::GeometryType::Polygon)
            mPolicies.push_back(std::make_unique<PolygonFeatureDrawingPolicy>(symbol));
        else if (mFeatureType == common::GeometryType::MultiPolygon)
            mPolicies.push_back(std::make_unique<MultiPolygonFeatureDrawingPolicy>(symbol));
        break;
    }
    default:
        return;
    }
}
