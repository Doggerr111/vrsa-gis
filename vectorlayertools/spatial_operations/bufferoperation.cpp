#include "bufferoperation.h"
#include "vector/vectorlayer.h"
#include "geometry/geometryconverter.h"
#include "vectorlayertools/vectorlayercreator.h"

vrsa::vector::BufferOperation::BufferOperation(VectorLayer *inputLayer, const common::SpatialOperationDTO &dto,
                                               VectorLayerCreator *creator)
    : SpatialOperation(dto, creator, inputLayer)
{

}

std::unique_ptr<geos::geom::Geometry> vrsa::vector::BufferOperation::executeGeos(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2)
{
    return geom1->buffer(mParams.distance);
}

void vrsa::vector::BufferOperation::processLayers(VectorLayer *firstLayer, VectorLayer *secondLayer)
{
    std::vector<gdalwrapper::OgrGeometryPtr> geom;
    for (const auto& feature: firstLayer->getFeatures())
    {
        auto featureGeom = feature->getOGRGeometry();
        auto geosFeatureGeom = geometry::GeometryConverter::createGEOSFromOGR(featureGeom);
        geom.emplace_back(geometry::GeometryConverter::createOGRfromGeos(executeGeos(geosFeatureGeom.get(), nullptr).get()));
    }
    auto ds = mCreator->createGDALDatasetFromGeometries(mParams.outputPath, std::move(geom), firstLayer);
    ds.reset();
    mCreator->emitLayerReadingRequest(mParams.outputPath);

}
