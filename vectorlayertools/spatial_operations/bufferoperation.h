#ifndef BUFFEROPERATION_H
#define BUFFEROPERATION_H
#include "vectorlayertools/spatial_operations/spatialoperation.h"
namespace vrsa
{
namespace vector
{
class VectorLayer;
class BufferOperation : public SpatialOperation
{
public:
    BufferOperation(VectorLayer* inputLayer, const common::SpatialOperationDTO& dto,
                                                vector::VectorLayerCreator* creator);

    // SpatialOperation interface
public:
    QString name() const override { return tr("Расчет буферной зоны"); };
    bool requiresTwoInputs() const noexcept override { return false; };
    SpatialOperationType type() const noexcept override { return SpatialOperationType::Buffer; };
protected:
    void processLayers(VectorLayer *firstLayer, VectorLayer *secondLayer) override;
    std::unique_ptr<geos::geom::Geometry> executeGeos(const geos::geom::Geometry *geom1,
                                                      const geos::geom::Geometry *geom2) override;
};

}
}

#endif // BUFFEROPERATION_H
