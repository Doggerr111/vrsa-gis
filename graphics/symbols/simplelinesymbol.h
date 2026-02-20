#ifndef SIMPLELINESYMBOL_H
#define SIMPLELINESYMBOL_H

#include "simplesymbol.h"

namespace vrsa
{
namespace graphics
{

class SimpleLineSymbol: public SimpleSymbol
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

    static std::unique_ptr<SimpleLineSymbol> createDefaultSymbol();

    /**
     * @english
     * @brief Returns pen configured with width converted to correct units
     * @endenglish
     * @russian
     * @brief Возвращает перо с шириной, конвертированной в нужные единицы
     * @endrussian
     */
    QPen pen() const;
    /**
     * @english
     * @brief Returns X offset converted to correct units
     * @endenglish
     * @russian
     * @brief Возвращает смещение по X, конвертированное в нужные единицы
     * @endrussian
     */
    double getXOffSet() const;
    /**
     * @english
     * @brief Returns Y offset converted to correct units
     * @endenglish
     * @russian
     * @brief Возвращает смещение по Y, конвертированное в нужные единицы
     * @endrussian
     */
    double getYOffSet() const;



public:
    //borders
    Qt::PenStyle borderStyle = Qt::SolidLine;
    Qt::PenJoinStyle joinStyle = Qt::PenJoinStyle::RoundJoin;
    Qt::PenCapStyle capStyle = Qt::PenCapStyle::RoundCap;
    QColor borderColor = Qt::black;
    double borderWidth = 0.25;
    QVector<double> customDashPattern;

    double xOffset = 0.0;
    double yOffset = 0.0;
};
}
}

#endif // SIMPLELINESYMBOL_H
