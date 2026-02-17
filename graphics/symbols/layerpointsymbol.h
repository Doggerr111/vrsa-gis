#ifndef LAYERPOINTSYMBOL_H
#define LAYERPOINTSYMBOL_H

#include "layersymbol.h"

namespace vrsa
{
namespace graphics
{

class LayerPointSymbol : public LayerSymbol
{
public:
    LayerPointSymbol(std::unique_ptr<Symbol> baseSymbol);



    // Symbol interface
public:
    common::SymbolType type()  const noexcept override { return common::SymbolType::LayerPointSymbol; };
};

}
}
#endif // LAYERPOINTSYMBOL_H
