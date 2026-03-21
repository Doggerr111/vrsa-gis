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
    std::unique_ptr<geos::geom::Geometry> execute(const geos::geom::Geometry *geom1,
                                                  const geos::geom::Geometry *geom2) override;
    void executeOnLayer(VectorLayer *inputLayer) override;
    bool requiresTwoInputs() const override { return false; };
    QString name() const override { return ("Расчет буферной зоны"); };
    SpatialOperationType type() const override { return SpatialOperationType::Buffer; };
};

}
}

#endif // BUFFEROPERATION_H
