#ifndef SPATIALOPERATION_H
#define SPATIALOPERATION_H
#include "geos/geom/GeometryFactory.h"
#include "common/spatialoperation_dto.h"
#include <QString>
#include <QObject>
namespace vrsa
{
namespace vector
{
class VectorLayerCreator;
class VectorLayer;

enum class SpatialOperationType
{
    Unknown = 0,
    Buffer,
    Triangulation,
    VoronoiDiagramm,
    Intersection,
    Union,
    Difference,
    SymDifference
};
inline QString spatialOperationTypeToQString(SpatialOperationType type)
{
    switch (type)
    {
    case SpatialOperationType::Buffer:
        return "Буферная зона";
    case SpatialOperationType::Triangulation:
        return "Триангуляция";
    case SpatialOperationType::VoronoiDiagramm:
        return "Диаграмма Вороного";
    case SpatialOperationType::Intersection:
        return "Пересечение";
    case SpatialOperationType::Union:
        return "Объединение";
    case SpatialOperationType::Difference:
        return "Разность";
    case SpatialOperationType::SymDifference:
        return "Симметричная разность";
    case SpatialOperationType::Unknown:
    default:
        return "Неизвестная операция";
    }
}


class SpatialOperation: public QObject
{


public:
    virtual ~SpatialOperation() = default;
    SpatialOperation(const common::SpatialOperationDTO& dto, VectorLayerCreator* creator);
    // Общий интерфейс для всех операций
    virtual std::unique_ptr<geos::geom::Geometry> execute(const geos::geom::Geometry* geom1,
                                                          const geos::geom::Geometry* geom2 = nullptr) = 0;
    virtual void executeOnLayer(VectorLayer* inputLayer){ return; };
    virtual void executeOnLayers(VectorLayer* firstLayer, VectorLayer* secondLayer){ return; };
    void calculate()
    {
        switch (type())
        {
        case SpatialOperationType::Buffer:
        case SpatialOperationType::Triangulation:
        case SpatialOperationType::VoronoiDiagramm:
            executeOnLayer(mInputLayer);
            break;
        case SpatialOperationType::Intersection:
        case SpatialOperationType::Union:
        case SpatialOperationType::Difference:
        case SpatialOperationType::SymDifference:
            executeOnLayers(mInputLayer, mSecondLayer);
            break;
        default:
            break;
        }
    }
    void setInputLayer(vector::VectorLayer* layer) { mInputLayer = layer; }
    void setSecondInputLayer(vector::VectorLayer* layer) { mSecondLayer = layer; }
    virtual bool requiresTwoInputs() const { return false; }

    virtual QString name() const = 0;
    virtual SpatialOperationType type() const = 0;

protected:
    vector::VectorLayerCreator* mCreator;
    vector::VectorLayer* mInputLayer;
    vector::VectorLayer* mSecondLayer;
    common::SpatialOperationDTO mParams;
};

}
}

#endif // SPATIALOPERATION_H
