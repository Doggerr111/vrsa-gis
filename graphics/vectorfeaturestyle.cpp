#include "vectorfeaturestyle.h"
#include "simplepointsymbol.h"
#include "simplelinesymbol.h"
#include "simplepolygonsymbol.h"
#include "layersymbol.h"
vrsa::graphics::VectorFeatureStyle::VectorFeatureStyle()
{

}

vrsa::graphics::VectorFeatureStyle::VectorFeatureStyle(std::unique_ptr<Symbol> symbol)
    :mSymbol{std::move(symbol)}
{

}

std::unique_ptr<vrsa::graphics::VectorFeatureStyle> vrsa::graphics::VectorFeatureStyle::createDefaultVectorStyle(const common::GeometryType type)
{
    using namespace vrsa::common;

    auto layerSymbol = std::make_unique<LayerSymbol>();
    switch (type)
    {
    case GeometryType::Point:
    case GeometryType::MultiPoint:
    {
        layerSymbol->addChild(SimplePointSymbol::createDefaultSymbol());
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    case GeometryType::LineString:
    case GeometryType::MultiLineString:
    {
        layerSymbol->addChild(SimpleLineSymbol::createDefaultSymbol());
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    case GeometryType::Polygon:
    case GeometryType::MultiPolygon:
    {
        layerSymbol->addChild(SimplePolygonSymbol::createDefaultSymbol());
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    default:
        return{};
    }
}


