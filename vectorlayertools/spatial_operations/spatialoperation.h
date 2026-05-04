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

class SpatialOperation: public QObject
{

public:
    SpatialOperation(const common::SpatialOperationDTO& dto, VectorLayerCreator *creator,
                     VectorLayer *first   = nullptr, VectorLayer *second = nullptr)
        : mParams{dto},
          mCreator{creator},
          mFirstLayer{first},
          mSecondLayer{second} {};
    virtual ~SpatialOperation() = default;

    void execute()
    {
        if (validateInputs())
            processLayers(mFirstLayer, mSecondLayer);
    }

    void setFirst (vector::VectorLayer* layer) noexcept { mFirstLayer = layer; }
    void setSecond(vector::VectorLayer* layer) noexcept { mSecondLayer = layer;}

    virtual QString name()              const = 0;
    virtual bool requiresTwoInputs()    const noexcept = 0;
    virtual SpatialOperationType type() const noexcept = 0;

protected:
    virtual std::unique_ptr<geos::geom::Geometry> executeGeos(const geos::geom::Geometry* geom1,
                                                              const geos::geom::Geometry* geom2 = nullptr) = 0;
    virtual void processLayers(VectorLayer* firstLayer, VectorLayer* secondLayer = nullptr) = 0;

protected:
    vector::VectorLayerCreator* mCreator;
    common::SpatialOperationDTO mParams;
    vector::VectorLayer* mFirstLayer  = nullptr;
    vector::VectorLayer* mSecondLayer = nullptr;

private:
    bool validateInputs() const noexcept
    {
        if (!mFirstLayer) return false;
        if (requiresTwoInputs() && !mSecondLayer) return false;
        return true;
    }
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

}
}

#endif // SPATIALOPERATION_H
