#ifndef SYMBOL_H
#define SYMBOL_H
#include <QColor>
#include "common/GisDefines.h"
#include <memory>
namespace vrsa
{
namespace graphics
{

class Symbol
{
public:
    Symbol() = default;
    virtual ~Symbol() = default;
    virtual common::SymbolType type() const = 0;

    virtual bool canHaveChildren() const { return false; }
    virtual bool hasChildren() const { return false; }
    virtual int childCount() const noexcept { return 0; }
    virtual const Symbol* child(int index) const { return nullptr; }
    virtual Symbol* child(int index) { return nullptr; }
    virtual void addChild(std::unique_ptr<Symbol>) {}
    virtual void removeChild(int index) {}



};

}
}

#endif // SYMBOL_H
