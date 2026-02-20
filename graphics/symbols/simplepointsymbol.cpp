#include "simplepointsymbol.h"
#include <QPen>
#include <QRandomGenerator>
#include "graphicsutils.h"


std::unique_ptr<vrsa::graphics::SimplePointSymbol> vrsa::graphics::SimplePointSymbol::createDefaultSymbol()
{
    auto pointSymbol = std::make_unique<SimplePointSymbol>();
    pointSymbol->borderColor = GraphicsUtils::generateColor();
    pointSymbol->fillColor = GraphicsUtils::generateColor();
    return pointSymbol;
}

QPen vrsa::graphics::SimplePointSymbol::pen() const
{
    QPen pen;
    pen.setColor(borderColor);
    pen.setStyle(borderStyle);
    pen.setJoinStyle(joinStyle);
    pen.setCapStyle(capStyle);
    pen.setWidthF(GraphicsUtils::convertToPixels(borderWidth, unitType));
    return pen;
}

QBrush vrsa::graphics::SimplePointSymbol::brush() const
{
    QBrush brush;
    brush.setColor(fillColor);
    brush.setStyle(fillStyle);
    return brush;
}

double vrsa::graphics::SimplePointSymbol::getPointSize() const
{
    return GraphicsUtils::convertToPixels(pointSize, unitType);
}

double vrsa::graphics::SimplePointSymbol::getXOffSet() const
{
    return GraphicsUtils::convertToPixels(xOffset, unitType);
}

double vrsa::graphics::SimplePointSymbol::getYOffSet() const
{
    return GraphicsUtils::convertToPixels(yOffset, unitType);
}
