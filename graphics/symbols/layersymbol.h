#ifndef LAYERSYMBOL_H
#define LAYERSYMBOL_H

#include "symbol.h"
#include "common/gisexceptions.h"
#include <QDebug>
namespace vrsa
{
namespace graphics
{


class LayerSymbol : public Symbol
{
public:
    LayerSymbol(): Symbol("Layer Symbol"){}
    LayerSymbol(const QString& name): Symbol(name){}
    LayerSymbol(const LayerSymbol& other)
        : Symbol(other)
    {
        for (const auto& child : other.mSymbols)
        {
            mSymbols.push_back(child->clone());
        }
    };
    LayerSymbol& operator=(const LayerSymbol& other)
    {
        if (this != &other)
        {
            Symbol::operator=(other);  // копируем базовую часть
            mSymbols.clear();
            for (const auto& child : other.mSymbols)
                mSymbols.push_back(child->clone());
        }
        return *this;
    }

    // Symbol interface
public:
    inline common::SymbolType type() const noexcept override { return common::SymbolType::LayerSymbol; };
    std::unique_ptr<Symbol> clone() const override { return std::make_unique<LayerSymbol>(*this); };
    inline bool canHaveChildren() const noexcept override { return true; };
    bool hasChildren() const override
    {
        return !mSymbols.empty();
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
    virtual std::unique_ptr<Symbol> takeChild(int index)
    {
        if (index < 0 || index >= mSymbols.size())
            return nullptr;

        auto result = std::move(mSymbols[index]);
        mSymbols.erase(mSymbols.begin() + index);
        return result;
    }
    virtual void insertChild(int index, std::unique_ptr<Symbol> child)
    {
        mSymbols.insert(mSymbols.begin() + index, std::move(child));
    }

    virtual void swapChildren(int index1, int index2)
    {
        for (const auto& s: mSymbols)
            qDebug()<<s.get();
        qDebug()<< "swaping:"+ QString::number(index1) + " and " + QString::number(index2);
         std::swap(mSymbols[index1], mSymbols[index2]);
         for (const auto& s: mSymbols)
             qDebug()<<s.get();
    }

    int indexOf(Symbol* child) const
    {
        for (size_t i = 0; i < mSymbols.size(); ++i)
        {
            if (mSymbols[i].get() == child)
                return static_cast<int>(i);

        }
        return -1; // Не найден
    }

    void reverseChildren()
    {
        std::reverse(mSymbols.begin(), mSymbols.end());
    }
protected:
    std::vector<std::unique_ptr<Symbol>> mSymbols;

};


class LayerPointSymbol : public LayerSymbol
{
public:
    LayerPointSymbol(): LayerSymbol("Point Layer Symbol"){}
    LayerPointSymbol(const LayerPointSymbol& other): LayerSymbol(other){qDebug()<<"operator= LayerPointSymbol";};
    LayerPointSymbol& operator=(const LayerPointSymbol& other)
    {
        qDebug()<<"operator= LayerPointSymbol";
        if (this != &other)
            LayerSymbol::operator=(other);  // копируем базовую часть
        return *this;
    }


    // Symbol interface
public:
    inline common::SymbolType type() const noexcept override { return common::SymbolType::LayerPointSymbol; };
    std::unique_ptr<Symbol> clone() const override { return std::make_unique<LayerPointSymbol>(*this); };
    void addChild(std::unique_ptr<Symbol> symbol) override
    {
        if (isPointType(symbol->type()))
        {
            mSymbols.push_back(std::move(symbol));
            return;
        }
        throw common::SymbolLayerGeometryException(mName.toStdString(), symbol->mName.toStdString());
    };

private:
    bool isPointType(common::SymbolType type)
    {
        return type == common::SymbolType::SimplePointSymbol ||
               type == common::SymbolType::LayerPointSymbol;
    }

};


class LayerLineSymbol : public LayerSymbol
{
public:
    LayerLineSymbol(): LayerSymbol("Line Layer Symbol"){}
    LayerLineSymbol(const LayerLineSymbol& other): LayerSymbol(other){};
    LayerLineSymbol& operator=(const LayerLineSymbol& other)
    {
        if (this != &other)
            LayerSymbol::operator=(other);  // копируем базовую часть
        return *this;
    }


    // Symbol interface
public:
    inline common::SymbolType type() const noexcept override { return common::SymbolType::LayerLineSymbol; };
    std::unique_ptr<Symbol> clone() const override { return std::make_unique<LayerLineSymbol>(*this); };

    void addChild(std::unique_ptr<Symbol> symbol) override
    {
        if (isLineType(symbol->type()))
        {
            mSymbols.push_back(std::move(symbol));
            return;
        }
        throw common::SymbolLayerGeometryException(mName.toStdString(), symbol->mName.toStdString());
    };

private:
    bool isLineType(common::SymbolType type)
    {
        return type == common::SymbolType::SimpleLineSymbol ||
               type == common::SymbolType::LayerLineSymbol;
    }

};



class LayerPolygonSymbol : public LayerSymbol
{
public:
    LayerPolygonSymbol(): LayerSymbol("Polygon Layer Symbol"){}
    LayerPolygonSymbol(const LayerPolygonSymbol& other): LayerSymbol(other){};
    LayerPolygonSymbol& operator=(const LayerPolygonSymbol& other)
    {
        if (this != &other)
            LayerSymbol::operator=(other);  // копируем базовую часть
        return *this;
    }



    // Symbol interface
public:
    inline common::SymbolType type() const noexcept override { return common::SymbolType::LayerPolygonSymbol; };
    std::unique_ptr<Symbol> clone() const override { return std::make_unique<LayerPolygonSymbol>(*this); };

    void addChild(std::unique_ptr<Symbol> symbol) override
    {
        if (isPolyType(symbol->type()))
        {
            mSymbols.push_back(std::move(symbol));
            return;
        }
        throw common::SymbolLayerGeometryException(mName.toStdString(), symbol->mName.toStdString());
    };

private:
    bool isPolyType(common::SymbolType type)
    {
        return type == common::SymbolType::SimplePolygonSymbol ||
               type == common::SymbolType::LayerPolygonSymbol;
    }

};



}
}

#endif // LAYERSYMBOL_H
