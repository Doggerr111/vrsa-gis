#include "simplelinesymbol.h"
#include "graphicsutils.h"
#include <QPen>

std::unique_ptr<vrsa::graphics::SimpleLineSymbol> vrsa::graphics::SimpleLineSymbol::createDefaultSymbol()
{
    auto lineSymbol = std::make_unique<SimpleLineSymbol>();
    lineSymbol->borderColor = GraphicsUtils::generateColor();
    return lineSymbol;
}

QPen vrsa::graphics::SimpleLineSymbol::pen() const
{
    QPen pen;
    pen.setColor(borderColor);
    pen.setStyle(borderStyle);
    pen.setJoinStyle(joinStyle);
    pen.setCapStyle(capStyle);
    pen.setWidthF(GraphicsUtils::convertToPixels(borderWidth, unitType));
    return pen;
}

double vrsa::graphics::SimpleLineSymbol::getXOffSet() const
{
    return GraphicsUtils::convertToPixels(xOffset, unitType);
}

double vrsa::graphics::SimpleLineSymbol::getYOffSet() const
{
    return GraphicsUtils::convertToPixels(yOffset, unitType);
}
