#ifndef SYMDIFFERENCEOPERATION_H
#define SYMDIFFERENCEOPERATION_H
#include "vectorlayertools/spatial_operations/spatialoperation.h"

namespace vrsa
{
namespace vector
{
class VectorLayer;
class SymDifferenceOperation: public SpatialOperation
{
public:
    SymDifferenceOperation(VectorLayer* inputLayer, VectorLayer* overlayLayer, const common::SpatialOperationDTO& dto,
                                                vector::VectorLayerCreator* creator);

    // SpatialOperation interface
public:
    QString name() const override { return tr("Симметрическая разность"); };
    bool requiresTwoInputs() const noexcept override { return true; };
    SpatialOperationType type() const noexcept override { return SpatialOperationType::SymDifference; };
protected:
    void processLayers(VectorLayer *firstLayer, VectorLayer *secondLayer) override;
    std::unique_ptr<geos::geom::Geometry> executeGeos(const geos::geom::Geometry *geom1,
                                                      const geos::geom::Geometry *geom2) override;
};
}
}

#endif // SYMDIFFERENCEOPERATION_H
