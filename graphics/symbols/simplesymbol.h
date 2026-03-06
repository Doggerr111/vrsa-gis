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
    SimpleSymbol()
        : Symbol("Simple Symbol"),
          unitType{common::StyleUnit::Millimeters},
          opacity{1.0} {}
    SimpleSymbol(const QString& name)
        : Symbol(name),
        unitType{common::StyleUnit::Millimeters},
        opacity{1.0} {};
    virtual ~SimpleSymbol() = default;

public:
    common::StyleUnit unitType = common::StyleUnit::Millimeters;
    double opacity = 1.0 ;

    // Symbol interface
public:
    common::SymbolType type() const noexcept override { return common::SymbolType::SimpleSymbol; };
    std::unique_ptr<Symbol> clone() const override { return std::make_unique<SimpleSymbol>(*this); };
};
}
}

#endif // SIMPLESYMBOL_H
