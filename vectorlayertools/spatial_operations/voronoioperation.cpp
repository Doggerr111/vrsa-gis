#include "voronoioperation.h"
#include "vector/vectorlayer.h"
#include "geometry/geometryconverter.h"
#include "vectorlayertools/vectorlayercreator.h"
#include "geos/triangulate/VoronoiDiagramBuilder.h"
#include "gdal/geometrytypeconverter.h"
#include <geos/triangulate/quadedge/QuadEdgeSubdivision.h>

vrsa::vector::VoronoiOperation::VoronoiOperation(VectorLayer *inputLayer,
                                                 const common::SpatialOperationDTO &dto, VectorLayerCreator *creator)
    : SpatialOperation(dto, creator)
{
    if (inputLayer) setInputLayer(inputLayer);
}

std::unique_ptr<geos::geom::Geometry> vrsa::vector::VoronoiOperation::execute(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2)
{
    auto builder = geos::triangulate::VoronoiDiagramBuilder();
    builder.setSites(*geom1);
    builder.setTolerance(mParams.tolerance);
    auto factory = geom1->getFactory();
    return builder.getDiagram(*factory);
}

void vrsa::vector::VoronoiOperation::executeOnLayer(VectorLayer *inputLayer)
{
    std::vector<gdalwrapper::OgrGeometryPtr> geom;
    if (!inputLayer) return;
    if (!mParams.isValid())
    {
        VRSA_ERROR("GEOS", "Invalid parameters");
        return;
    }
    auto geosPoints = geometry::GeometryConverter::createGEOSPointCollectionFromVectorLayer(inputLayer);
    if (!geosPoints)
    {
        VRSA_ERROR("GEOS", "Invalid unput layer geometry");
        return;
    }
    auto triangulationGeos = execute(geosPoints.get(), nullptr);
    if (!triangulationGeos)
    {
        VRSA_ERROR("GEOS", "Building of Voronoi diagram has failed");
    }
    //GEOMETRY COLLECTION (OF POLYGONS)
    auto triangulationOGR = geometry::GeometryConverter::createOGRfromGeos(triangulationGeos.get());
    auto ds = mCreator->createGDALDatasetFromGeometryCollection(mParams.outputPath, std::move(triangulationOGR), inputLayer);
    ds.reset();
    mCreator->emitLayerReadingRequest(mParams.outputPath);
}
