#ifndef SIMPLEPOLYGONSYMBOL_H
#define SIMPLEPOLYGONSYMBOL_H

#include "symbol.h"
#include "QImage"

namespace vrsa
{
namespace graphics
{

class SimplePolygonSymbol : public Symbol
{
public:
    SimplePolygonSymbol() = default;
    SimplePolygonSymbol(const SimplePolygonSymbol&) = default;
    SimplePolygonSymbol(SimplePolygonSymbol&&) = default;
    SimplePolygonSymbol& operator=(const SimplePolygonSymbol&) = default;
    SimplePolygonSymbol& operator=(SimplePolygonSymbol&&) = default;
    ~SimplePolygonSymbol() = default;

    // Symbol interface
public:
    common::SymbolType type() const noexcept override { return common::SymbolType::SimplePolygonSymbol; };



public:
    //fill
    QColor fillColor;
    Qt::BrushStyle fillStyle;
    QImage imageMarker; //hmm

    //borders
    Qt::PenStyle borderStyle;
    Qt::PenJoinStyle joinStyle;
    Qt::PenCapStyle capStyle;
    QColor borderColor;
    double borderWidth;

    double xOffset;
    double yOffset;
    double rotation;
    double pointSize;
    common::PointSymbolType pointType;
};
}
}

#endif // SIMPLEPOLYGONSYMBOL_H
