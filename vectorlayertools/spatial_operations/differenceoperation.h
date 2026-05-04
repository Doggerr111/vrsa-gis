#ifndef DIFFERENCEOPERATION_H
#define DIFFERENCEOPERATION_H
#include "vectorlayertools/spatial_operations/spatialoperation.h"

namespace vrsa
{
namespace vector
{
class VectorLayer;
class DifferenceOperation : public SpatialOperation
{
public:
    DifferenceOperation(VectorLayer* inputLayer, VectorLayer* overlayLayer, const common::SpatialOperationDTO& dto,
                                                vector::VectorLayerCreator* creator);

    // SpatialOperation interface
public:
    QString name() const override { return tr("Разность"); };
    bool requiresTwoInputs() const noexcept override { return true; };
    SpatialOperationType type() const noexcept override { return SpatialOperationType::Difference; };
protected:
    void processLayers(VectorLayer *firstLayer, VectorLayer *secondLayer) override;
    std::unique_ptr<geos::geom::Geometry> executeGeos(const geos::geom::Geometry *geom1,
                                                      const geos::geom::Geometry *geom2) override;

};
}
}

class DifferenceOperation
{
public:
    DifferenceOperation();
};

#endif // DIFFERENCEOPERATION_H
