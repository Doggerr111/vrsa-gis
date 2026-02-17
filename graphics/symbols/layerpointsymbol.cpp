#include "layerpointsymbol.h"




vrsa::graphics::LayerPointSymbol::LayerPointSymbol(std::unique_ptr<Symbol> baseSymbol)
    : LayerSymbol()
{
    if (baseSymbol)
        mSymbols.push_back(std::move(baseSymbol));
}
