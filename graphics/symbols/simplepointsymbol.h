#ifndef SIMPLEPOINTSYMBOL_H
#define SIMPLEPOINTSYMBOL_H

#include "simplesymbol.h"
#include <QImage>

namespace vrsa
{
namespace graphics
{

class SimplePointSymbol: public SimpleSymbol
{
public:
    SimplePointSymbol() = default;
    SimplePointSymbol(const SimplePointSymbol&) = default;
    SimplePointSymbol(SimplePointSymbol&&) = default;
    SimplePointSymbol& operator=(const SimplePointSymbol&) = default;
    SimplePointSymbol& operator=(SimplePointSymbol&&) = default;
    ~SimplePointSymbol() = default;

    // Symbol interface
public:
    common::SymbolType type() const noexcept override { return common::SymbolType::SimplePointSymbol; };

    QPen pen() const;
    QBrush brush() const;



public:
    //fill
    QColor fillColor = Qt::black;
    Qt::BrushStyle fillStyle = Qt::BrushStyle::SolidPattern;
    QImage imageMarker;

    //borders
    Qt::PenStyle borderStyle = Qt::SolidLine;
    Qt::PenJoinStyle joinStyle;
    Qt::PenCapStyle capStyle;
    QColor borderColor = Qt::black;
    double borderWidth;

    double xOffset;
    double yOffset;
    double rotation;
    double pointSize = 20;
    common::PointSymbolType pointType;

};

}
}
#endif // SIMPLEPOINTSYMBOL_H
