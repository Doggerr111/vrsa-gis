#include "simplepolygonsymbol.h"
#include "graphicsutils.h"
#include <QPen>
std::unique_ptr<vrsa::graphics::SimplePolygonSymbol> vrsa::graphics::SimplePolygonSymbol::createDefaultSymbol()
{
    auto polySymbol = std::make_unique<SimplePolygonSymbol>();
    polySymbol->borderColor = GraphicsUtils::generateColor();
    polySymbol->fillColor = GraphicsUtils::generateColor();
    return polySymbol;
}

QPen vrsa::graphics::SimplePolygonSymbol::pen() const
{
    QPen pen;
    pen.setColor(borderColor);
    pen.setStyle(borderStyle);
    pen.setJoinStyle(joinStyle);
    pen.setCapStyle(capStyle);
    pen.setWidthF(GraphicsUtils::convertToPixels(borderWidth, unitType));
    return pen;
}

QBrush vrsa::graphics::SimplePolygonSymbol::brush() const
{
    QBrush brush;
    brush.setColor(fillColor);
    brush.setStyle(fillStyle);
    return brush;
}

double vrsa::graphics::SimplePolygonSymbol::getXOffSet() const
{
    return GraphicsUtils::convertToPixels(xOffset, unitType);
}

double vrsa::graphics::SimplePolygonSymbol::getYOffSet() const
{
    return GraphicsUtils::convertToPixels(yOffset, unitType);
}
