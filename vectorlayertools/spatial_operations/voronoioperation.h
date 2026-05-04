#ifndef VORONOIOPERATION_H
#define VORONOIOPERATION_H

#include "spatialoperation.h"

#include "vectorlayertools/spatial_operations/spatialoperation.h"
namespace vrsa
{
namespace vector
{
class VectorLayer;
class VoronoiOperation : public SpatialOperation
{
public:
    VoronoiOperation(VectorLayer* inputLayer, const common::SpatialOperationDTO& dto,
                                                vector::VectorLayerCreator* creator);

    // SpatialOperation interface
public:
    QString name() const override { return tr("Диаграмма Вороного"); };
    bool requiresTwoInputs() const noexcept override { return false; };
    SpatialOperationType type() const noexcept override { return SpatialOperationType::VoronoiDiagramm; };
protected:
    void processLayers(VectorLayer *firstLayer, VectorLayer *secondLayer) override;
    std::unique_ptr<geos::geom::Geometry> executeGeos(const geos::geom::Geometry *geom1,
                                                      const geos::geom::Geometry *geom2) override;
};

}
}


#endif // VORONOIOPERATION_H
