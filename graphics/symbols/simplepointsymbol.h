#ifndef SIMPLEPOINTSYMBOL_H
#define SIMPLEPOINTSYMBOL_H

#include "simplesymbol.h"
#include <QImage>


namespace vrsa
{
namespace graphics
{

class SimplePointSymbol: public SimpleSymbol
{
public:
    SimplePointSymbol() = default;
    SimplePointSymbol(const SimplePointSymbol&) = default;
    SimplePointSymbol(SimplePointSymbol&&) = default;
    SimplePointSymbol& operator=(const SimplePointSymbol&) = default;
    SimplePointSymbol& operator=(SimplePointSymbol&&) = default;
    ~SimplePointSymbol() = default;

    // Symbol interface
public:
    common::SymbolType type() const noexcept override { return common::SymbolType::SimplePointSymbol; };

    static std::unique_ptr<SimplePointSymbol> createDefaultSymbol();

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
     * @brief Returns brush for filling (color and style)
     * @endenglish
     * @russian
     * @brief Возвращает кисть для заливки (цвет и стиль)
     * @endrussian
     */
    QBrush brush() const;
    /**
     * @english
     * @brief Returns point size converted to correct units
     * @endenglish
     * @russian
     * @brief Возвращает размер точки, конвертированный в нужные единицы
     * @endrussian
     */
    double getPointSize() const; //return pointSize in correct units !!
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
    //fill
    QColor fillColor = Qt::black;
    Qt::BrushStyle fillStyle = Qt::BrushStyle::SolidPattern;
    QImage imageMarker;

    //borders
    Qt::PenStyle borderStyle = Qt::SolidLine;
    Qt::PenJoinStyle joinStyle = Qt::PenJoinStyle::RoundJoin;
    Qt::PenCapStyle capStyle = Qt::PenCapStyle::RoundCap;
    QColor borderColor = Qt::black;
    double borderWidth = 0.2;

    double xOffset = 0.0;
    double yOffset = 0.0;
    double rotation = 0.0;
    double pointSize = 1;
    common::PointSymbolType pointType = common::PointSymbolType::Circle;

};

}
}
#endif // SIMPLEPOINTSYMBOL_H
