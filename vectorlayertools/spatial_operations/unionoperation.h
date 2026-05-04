#ifndef UNIONOPERATION_H
#define UNIONOPERATION_H
#include "vectorlayertools/spatial_operations/spatialoperation.h"

namespace vrsa
{
namespace vector
{
class VectorLayer;
class UnionOperation: public SpatialOperation
{
public:
    UnionOperation(VectorLayer* inputLayer, VectorLayer* overlayLayer, const common::SpatialOperationDTO& dto,
                                                vector::VectorLayerCreator* creator);

    // SpatialOperation interface
public:
    QString name() const override { return tr("Объединение"); };
    bool requiresTwoInputs() const noexcept override { return true; };
    SpatialOperationType type() const noexcept override { return SpatialOperationType::Union; };
protected:
    void processLayers(VectorLayer *firstLayer, VectorLayer *secondLayer) override;
    std::unique_ptr<geos::geom::Geometry> executeGeos(const geos::geom::Geometry *geom1,
                                                      const geos::geom::Geometry *geom2) override;
};
}
}

#endif // UNIONOPERATION_H
