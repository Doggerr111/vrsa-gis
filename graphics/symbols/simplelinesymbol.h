#ifndef SIMPLELINESYMBOL_H
#define SIMPLELINESYMBOL_H

#include "symbol.h"

namespace vrsa
{
namespace graphics
{

class SimpleLineSymbol: public Symbol
{
public:
    SimpleLineSymbol() = default;
    SimpleLineSymbol(const SimpleLineSymbol&) = default;
    SimpleLineSymbol(SimpleLineSymbol&&) = default;
    SimpleLineSymbol& operator=(const SimpleLineSymbol&) = default;
    SimpleLineSymbol& operator=(SimpleLineSymbol&&) = default;
    ~SimpleLineSymbol() = default;

    // Symbol interface
public:
    common::SymbolType type() const noexcept override { return common::SymbolType::SimpleLineSymbol; };



public:
    //borders
    Qt::PenStyle borderStyle;
    Qt::PenJoinStyle joinStyle;
    Qt::PenCapStyle capStyle;
    QColor borderColor;
    double borderWidth;
    QVector<double> customDashPattern;

    double xOffset;
    double yOffset;
};
}
}

#endif // SIMPLELINESYMBOL_H
