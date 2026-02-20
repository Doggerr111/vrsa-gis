#ifndef SIMPLESYMBOL_H
#define SIMPLESYMBOL_H

#include "symbol.h"

namespace vrsa
{
namespace graphics
{

class SimpleSymbol : public Symbol
{
public:
    SimpleSymbol();

public:
    common::StyleUnit unitType = common::StyleUnit::Millimeters;
    double opacity = 1.0 ;

    // Symbol interface
public:
    common::SymbolType type() const noexcept override { return common::SymbolType::SimpleSymbol; };
};
}
}

#endif // SIMPLESYMBOL_H
