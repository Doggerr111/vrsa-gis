#include "simplepointsymbol.h"
#include <QPen>




vrsa::graphics::SimplePointSymbol::SimplePointSymbol()
{

}

QPen vrsa::graphics::SimplePointSymbol::pen() const
{
    QPen pen;
    pen.setColor(borderColor);
    pen.setStyle(borderStyle);
    pen.setJoinStyle(joinStyle);
    pen.setCapStyle(capStyle);
    pen.setWidthF(borderWidth);
    return pen;
}

QBrush vrsa::graphics::SimplePointSymbol::brush() const
{
    QBrush brush;
    brush.setColor(fillColor);
    brush.setStyle(fillStyle);
    return brush;
}
