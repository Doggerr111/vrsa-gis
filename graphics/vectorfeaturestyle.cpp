#include "vectorfeaturestyle.h"
#include "simplepointsymbol.h"
#include "simplelinesymbol.h"
#include "simplepolygonsymbol.h"
#include "layersymbol.h"
#include "graphics/vertexhandle.h"
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
    switch (type)
    {
    case GeometryType::Point:
    case GeometryType::MultiPoint:
    {
        auto layerSymbol = std::make_unique<LayerPointSymbol>();
        layerSymbol->addChild(SimplePointSymbol::createDefaultSymbol());
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    case GeometryType::LineString:
    case GeometryType::MultiLineString:
    {
        auto layerSymbol = std::make_unique<LayerLineSymbol>();
        layerSymbol->addChild(SimpleLineSymbol::createDefaultSymbol());
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    case GeometryType::Polygon:
    case GeometryType::MultiPolygon:
    {
        auto layerSymbol = std::make_unique<LayerPolygonSymbol>();
        layerSymbol->addChild(SimplePolygonSymbol::createDefaultSymbol());
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    default:
        return{};
    }
}

std::unique_ptr<vrsa::graphics::VectorFeatureStyle> vrsa::graphics::VectorFeatureStyle::createSelectionVectorStyle(const VectorFeatureStyle *baseStyle)
{
    using namespace vrsa::common;
    if (!baseStyle)
        return{};
    if (!baseStyle->mSymbol)
        return{};
     //qDebug()<< "baseStyleSymbol - " << baseStyle->mSymbol.get();
    auto selectionStyleSymbol = baseStyle->mSymbol->clone();
    //qDebug()<< "selectionStyleSymbol - " << selectionStyleSymbol.get();
    updateBaseSymbolToSelectionSymbol(selectionStyleSymbol.get());
   // qDebug()<< "selectionStyleSymbol - " << selectionStyleSymbol.get();
    return std::make_unique<VectorFeatureStyle>(std::move(selectionStyleSymbol));

}

void vrsa::graphics::VectorFeatureStyle::updateBaseSymbolToSelectionSymbol(Symbol *symbol)
{
    if (!symbol)
        return;
    if (symbol->canHaveChildren())
    {
        for (int i=0; i<symbol->childCount(); ++i)
        {
            auto childSymbol = symbol->child(i);
            if (!childSymbol)
                return;
            updateBaseSymbolToSelectionSymbol(childSymbol);
        }
        return;
    }
    switch (symbol->type())
    {
    case common::SymbolType::SimplePointSymbol:
    {
        auto pointSymbol = static_cast<SimplePointSymbol*>(symbol);
        pointSymbol->pointSize = pointSymbol->pointSize * vrsa::common::SELECTION_SIZE_FACTOR;
        pointSymbol->borderWidth = pointSymbol->borderWidth * vrsa::common::SELECTION_WIDTH_FACTOR;
        pointSymbol->opacity = vrsa::common::SELECTION_OPACITY;
        pointSymbol->fillColor = vrsa::common::SELECTION_COLOR;
        pointSymbol->fillStyle = vrsa::common::SELECTION_BRUSH_STYLE;
        break;
    }
    case common::SymbolType::SimpleLineSymbol:
    {
        auto lineSymbol = static_cast<SimpleLineSymbol*>(symbol);
        lineSymbol->borderWidth = lineSymbol->borderWidth * vrsa::common::SELECTION_WIDTH_FACTOR;
        lineSymbol->opacity = vrsa::common::SELECTION_OPACITY;
        lineSymbol->borderColor = vrsa::common::SELECTION_COLOR;
        break;
    }
    case common::SymbolType::SimplePolygonSymbol:
    {
        auto polygonSymbol = static_cast<SimplePolygonSymbol*>(symbol);
        polygonSymbol->borderWidth = polygonSymbol->borderWidth * vrsa::common::SELECTION_WIDTH_FACTOR;
        polygonSymbol->opacity = vrsa::common::SELECTION_OPACITY;
        polygonSymbol->fillColor = vrsa::common::SELECTION_COLOR;
        polygonSymbol->fillStyle = vrsa::common::SELECTION_BRUSH_STYLE;
        break;
    }
    default:
        break;

    }
}


std::unique_ptr<vrsa::graphics::VectorFeatureStyle> vrsa::graphics::VectorFeatureStyle::createForDigitizing(const common::GeometryType type)
{
    using namespace vrsa::common;
    switch (type)
    {
    case GeometryType::Point:
    case GeometryType::MultiPoint:
    {
        auto layerSymbol = std::make_unique<LayerPointSymbol>();
        layerSymbol->addChild(SimplePointSymbol::createDefaultSymbol());
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    case GeometryType::LineString:
    case GeometryType::MultiLineString:
    {
        auto layerSymbol = std::make_unique<LayerLineSymbol>();
        auto lineSymbol = SimpleLineSymbol::createDefaultSymbol();
        lineSymbol->borderColor = Qt::red;
        lineSymbol->borderWidth = 0.1;
        lineSymbol->opacity = 0.5;
        layerSymbol->addChild(std::move(lineSymbol));
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    case GeometryType::Polygon:
    case GeometryType::MultiPolygon:
    {
        auto layerSymbol = std::make_unique<LayerPolygonSymbol>();
        auto polySymbol = SimplePolygonSymbol::createDefaultSymbol();
        polySymbol->borderColor = Qt::black;
        polySymbol->borderWidth = 0.1;
        polySymbol->fillColor = Qt::red;
        polySymbol->opacity = 0.5;
        layerSymbol->addChild(std::move(polySymbol));
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    default:
        return{};
    }
}

std::unique_ptr<vrsa::graphics::VectorFeatureStyle> vrsa::graphics::VectorFeatureStyle::createForSelection(const common::GeometryType type)
{
    using namespace common;
    switch (type)
    {
    case GeometryType::Point:
    case GeometryType::MultiPoint:
    {
//        auto layerSymbol = std::make_unique<LayerPointSymbol>();
//        layerSymbol->addChild(SimplePointSymbol::createDefaultSymbol());
//        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));
        break;

    }
    case GeometryType::LineString:
    case GeometryType::MultiLineString:
    {
//        auto layerSymbol = std::make_unique<LayerLineSymbol>();
//        auto lineSymbol = SimpleLineSymbol::createDefaultSymbol();
//        lineSymbol->borderColor = Qt::red;
//        lineSymbol->borderWidth = 0.1;
//        lineSymbol->opacity = 0.5;
//        layerSymbol->addChild(std::move(lineSymbol));
//        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));
        break;
    }
    case GeometryType::Polygon:
    case GeometryType::MultiPolygon:
    {
        auto layerSymbol = std::make_unique<LayerPolygonSymbol>();
        auto polySymbol = SimplePolygonSymbol::createDefaultSymbol();
        polySymbol->borderColor = Qt::black;
        polySymbol->borderWidth = 0.1;
        polySymbol->fillColor = common::SELECTION_COLOR;
        polySymbol->opacity = 0.5;
        polySymbol->fillStyle = common::SELECTION_BRUSH_STYLE;
        layerSymbol->addChild(std::move(polySymbol));
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    default:
        return{};
    }
    return {};
}

std::unique_ptr<vrsa::graphics::VectorFeatureStyle> vrsa::graphics::VectorFeatureStyle::createForMeasurement(const common::GeometryType type)
{
    return nullptr;
}

std::unique_ptr<vrsa::graphics::VectorFeatureStyle> vrsa::graphics::VectorFeatureStyle::createForPreview(const common::GeometryType type)
{
    return nullptr;
}

std::unique_ptr<vrsa::graphics::VectorFeatureStyle> vrsa::graphics::VectorFeatureStyle::createForRubberBands(const common::GeometryType type)
{
    using namespace common;
    switch (type)
    {
    case GeometryType::Point:
    case GeometryType::MultiPoint:
    {
        auto layerSymbol = std::make_unique<LayerPointSymbol>();
        auto point = SimplePointSymbol::createDefaultSymbol();
        point->fillColor = Qt::black;
        layerSymbol->addChild(std::move(point));

        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));
        break;

    }
    case GeometryType::LineString:
    case GeometryType::MultiLineString:
    {
        auto layerSymbol = std::make_unique<LayerLineSymbol>();
        auto outlineSymbol = SimpleLineSymbol::createDefaultSymbol();
        outlineSymbol->borderColor = Qt::black;
        outlineSymbol->borderWidth = 1;
        outlineSymbol->opacity = 1;
        layerSymbol->addChild(std::move(outlineSymbol));

        auto lineSymbol = SimpleLineSymbol::createDefaultSymbol();
        lineSymbol->borderColor = QColor(255, 255, 0);
        lineSymbol->borderWidth = 0.3;
        lineSymbol->opacity = 1;
        layerSymbol->addChild(std::move(lineSymbol));

        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));
        break;
    }
    case GeometryType::Polygon:
    case GeometryType::MultiPolygon:
    {
        auto layerSymbol = std::make_unique<LayerPolygonSymbol>();

        auto outPolySymbol = SimplePolygonSymbol::createDefaultSymbol();
        outPolySymbol->borderColor = Qt::black;
        outPolySymbol->borderWidth = 1;
        outPolySymbol->opacity = 1;
        outPolySymbol->fillStyle = Qt::NoBrush;
        layerSymbol->addChild(std::move(outPolySymbol));

        auto polySymbol = SimplePolygonSymbol::createDefaultSymbol();
        polySymbol->borderColor = QColor(255, 255, 0);
        polySymbol->borderWidth = 0.3;
        polySymbol->opacity = 1;
        polySymbol->fillStyle = Qt::NoBrush;
        layerSymbol->addChild(std::move(polySymbol));

        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));

    }
    default:
        return{};
    }
    return {};
}

std::unique_ptr<vrsa::graphics::VectorFeatureStyle> vrsa::graphics::VectorFeatureStyle::createForVertexHandles(const VertexState state)
{
    switch(state)
    {
    case VertexState::Normal:
    case VertexState::Disabled:
    {
        auto layerSymbol = std::make_unique<LayerPointSymbol>();
        auto vertexSymbol = SimplePointSymbol::createDefaultSymbol();
        vertexSymbol->fillColor = common::VERTEX_COLOR_NORMAL;
        layerSymbol->addChild(std::move(vertexSymbol));
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));
        break;
    }
    case VertexState::Pressed:
    {
        auto layerSymbol = std::make_unique<LayerPointSymbol>();
        auto vertexSymbol = SimplePointSymbol::createDefaultSymbol();
        vertexSymbol->fillColor = common::VERTEX_COLOR_PRESSED;
        layerSymbol->addChild(std::move(vertexSymbol));
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));
        break;
    }
    case VertexState::Hover:
    {
        auto layerSymbol = std::make_unique<LayerPointSymbol>();
        auto vertexSymbol = SimplePointSymbol::createDefaultSymbol();
        vertexSymbol->fillColor = common::VERTEX_COLOR_HOVER;
        layerSymbol->addChild(std::move(vertexSymbol));
        return std::make_unique<VectorFeatureStyle>(std::move(layerSymbol));
        break;
    }
    }
}





