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
    SimpleSymbol() = default;

public:
    common::StyleUnit unitType = common::StyleUnit::Millimeters;
    double opacity;

    // Symbol interface
public:
    common::SymbolType type() const noexcept override { return common::SymbolType::SimpleSymbol; };
};
}
}

#endif // SIMPLESYMBOL_H
