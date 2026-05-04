#ifndef TRIANGULATIONOPERATION_H
#define TRIANGULATIONOPERATION_H

#include "spatialoperation.h"

namespace vrsa
{
namespace vector
{
class VectorLayer;
class TriangulationOperation : public SpatialOperation
{
public:
    TriangulationOperation(VectorLayer* inputLayer, const common::SpatialOperationDTO& dto,
                                                vector::VectorLayerCreator* creator);

    // SpatialOperation interface
public:
    QString name() const override { return tr("Триангуляция"); };
    bool requiresTwoInputs() const noexcept override { return false; };
    SpatialOperationType type() const noexcept override { return SpatialOperationType::Triangulation; };
protected:
    void processLayers(VectorLayer *firstLayer, VectorLayer *secondLayer) override;
    std::unique_ptr<geos::geom::Geometry> executeGeos(const geos::geom::Geometry *geom1,
                                                      const geos::geom::Geometry *geom2) override;
};

}
}

#endif // TRIANGULATIONOPERATION_H
