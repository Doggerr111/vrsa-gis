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
    std::unique_ptr<geos::geom::Geometry> execute(const geos::geom::Geometry *geom1,
                                                  const geos::geom::Geometry *geom2) override;
    void executeOnLayer(VectorLayer *inputLayer) override;
    bool requiresTwoInputs() const override { return false; };
    QString name() const override { return ("Расчет триангуляции"); };
    SpatialOperationType type() const override { return SpatialOperationType::Triangulation; };
};

}
}

#endif // TRIANGULATIONOPERATION_H
