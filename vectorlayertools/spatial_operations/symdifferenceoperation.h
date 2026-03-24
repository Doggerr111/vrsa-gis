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
    std::unique_ptr<geos::geom::Geometry> execute(const geos::geom::Geometry *geom1,
                                                  const geos::geom::Geometry *geom2) override;
    void executeOnLayers(VectorLayer *firstLayer, VectorLayer *secondLayer) override;
    bool requiresTwoInputs() const override { return true; };
    QString name() const override { return ("Расчет пересечений"); };
    SpatialOperationType type() const override { return SpatialOperationType::Intersection; };
};
}
}

#endif // SYMDIFFERENCEOPERATION_H
