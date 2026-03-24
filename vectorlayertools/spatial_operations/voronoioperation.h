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
    std::unique_ptr<geos::geom::Geometry> execute(const geos::geom::Geometry *geom1,
                                                  const geos::geom::Geometry *geom2) override;
    void executeOnLayer(VectorLayer *inputLayer) override;
    bool requiresTwoInputs() const override { return false; };
    QString name() const override { return ("Расчет диаграммы Вороного"); };
    SpatialOperationType type() const override { return SpatialOperationType::VoronoiDiagramm; };
};

}
}


#endif // VORONOIOPERATION_H
