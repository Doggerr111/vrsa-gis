#ifndef INTERSECTIONOPERATION_H
#define INTERSECTIONOPERATION_H
#include "vectorlayertools/spatial_operations/spatialoperation.h"

namespace vrsa
{
namespace vector
{
class VectorLayer;
class IntersectionOperation : public SpatialOperation
{
public:
    IntersectionOperation(VectorLayer* inputLayer, VectorLayer* overlayLayer, const common::SpatialOperationDTO& dto,
                                                vector::VectorLayerCreator* creator);

    // SpatialOperation interface
public:
    QString name() const override { return tr("Пересечение"); };
    bool requiresTwoInputs() const noexcept override { return true; };
    SpatialOperationType type() const noexcept override { return SpatialOperationType::Intersection; };
protected:
    void processLayers(VectorLayer *firstLayer, VectorLayer *secondLayer) override;
    std::unique_ptr<geos::geom::Geometry> executeGeos(const geos::geom::Geometry *geom1,
                                                      const geos::geom::Geometry *geom2) override;
};
}
}
#endif // INTERSECTIONOPERATION_H
