#ifndef LAYERSYMBOL_H
#define LAYERSYMBOL_H

#include "symbol.h"

namespace vrsa
{
namespace graphics
{


class LayerSymbol : public Symbol
{
public:
    LayerSymbol() = default;

    // Symbol interface
public:
    inline common::SymbolType type() const noexcept override { return common::SymbolType::LayerSymbol; };
    inline bool canHaveChildren() const noexcept override { return true; };
    bool hasChildren() const override
    {
        if (mSymbols.size()>0)
            return true;
        return false;
    };
    int childCount() const noexcept override
    {
        return mSymbols.size();
    };
    Symbol *child(int index) override
    {
        return mSymbols.at(index).get();
    };
    const Symbol *child(int index) const override
    {
        return mSymbols.at(index).get();
    };

    void addChild(std::unique_ptr<Symbol> symbol) override
    {
        mSymbols.push_back(std::move(symbol));
    };
    void removeChild(int index) override
    {
        if (index < mSymbols.size())
            mSymbols.erase(mSymbols.begin() + index);
    };

protected:
    std::vector<std::unique_ptr<Symbol>> mSymbols;

    // Symbol interface
public:

};

}
}

#endif // LAYERSYMBOL_H
