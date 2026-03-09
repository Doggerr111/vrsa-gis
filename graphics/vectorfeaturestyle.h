#ifndef VECTORFEATURESTYLE_H
#define VECTORFEATURESTYLE_H
#include <QPen>
#include <QBrush>
#include <QRandomGenerator>
#include "common/GisDefines.h"
#include "calculations/unitconverter.h"
#include "symbol.h"
namespace vrsa
{
namespace graphics
{
enum class VertexState: uint8_t;

class VectorFeatureStyle
{
public:
    VectorFeatureStyle();
    VectorFeatureStyle(std::unique_ptr<Symbol> symbol);

    inline Symbol* getSymbol()
    {
        return mSymbol.get();
    }
private:
    std::unique_ptr<Symbol> mSymbol;
    
public:
    //вобще в фабрику бы отдельную это все ...

    //при загрузке слоя к нему применяется этот стиль:
    static std::unique_ptr<VectorFeatureStyle> createDefaultVectorStyle(const common::GeometryType type);
    //при выделении векторных объектов на сцене приняется этот стиль:
    static std::unique_ptr<VectorFeatureStyle> createSelectionVectorStyle(const VectorFeatureStyle* baseStyle);
    static void updateBaseSymbolToSelectionSymbol(Symbol *symbol);
    //для временных графических объектов
    static std::unique_ptr<VectorFeatureStyle> createForDigitizing(const common::GeometryType type);
    static std::unique_ptr<VectorFeatureStyle> createForSelection(const common::GeometryType type);
    static std::unique_ptr<VectorFeatureStyle> createForMeasurement(const common::GeometryType type);
    static std::unique_ptr<VectorFeatureStyle> createForPreview(const common::GeometryType type);
    static std::unique_ptr<VectorFeatureStyle> createForRubberBands(const common::GeometryType type);
    static std::unique_ptr<VectorFeatureStyle> createForVertexHandles(const VertexState state);
    static std::unique_ptr<VectorFeatureStyle> createForVertexMiddlePoints(const VertexState state);


private:
    QPen mPen;
    QBrush mBrush;

    double mPointSize;
    common::GeometryType mType;
    common::PointSymbolType mPointSymbolType;
};

}
}

#endif // VECTORFEATURESTYLE_H
