#include "triangulationoperation.h"
#include "vector/vectorlayer.h"
#include "geometry/geometryconverter.h"
#include "vectorlayertools/vectorlayercreator.h"
#include "geos/triangulate/DelaunayTriangulationBuilder.h"
#include "gdal/geometrytypeconverter.h"
#include <geos/triangulate/quadedge/QuadEdgeSubdivision.h>
vrsa::vector::TriangulationOperation::TriangulationOperation
            (VectorLayer *inputLayer, const common::SpatialOperationDTO &dto, VectorLayerCreator *creator)
     : SpatialOperation(dto, creator)
{
    if (inputLayer) setInputLayer(inputLayer);
}

std::unique_ptr<geos::geom::Geometry> vrsa::vector::TriangulationOperation::execute(const geos::geom::Geometry *geom1,
                                                                                    const geos::geom::Geometry *geom2)
{
    auto builder = geos::triangulate::DelaunayTriangulationBuilder();
    builder.setSites(*geom1);
    builder.setTolerance(mParams.tolerance);
    auto factory = geom1->getFactory();
    return builder.getTriangles(*factory);
}

void vrsa::vector::TriangulationOperation::executeOnLayer(VectorLayer *inputLayer)
{
    std::vector<gdalwrapper::OgrGeometryPtr> geom;
    if (!inputLayer) return;
    if (!mParams.isValid())
    {
        VRSA_ERROR("GEOS", "Invalid triangulation parameters");
        return;
    }
    //SINGLE MULTIPOINT
    auto geosPoints = geometry::GeometryConverter::createGEOSPointCollectionFromVectorLayer(inputLayer);
    //VRSA_DEBUG("GEOS", "CONVERTED POINTS GEOM:" + geosPoints->toString());
    if (!geosPoints)
    {
        VRSA_ERROR("GEOS", "Invalid unput layer geometry");
        return;
    }
    auto triangulationGeos = execute(geosPoints.get(), nullptr);
    //VRSA_DEBUG("GEOS", "TRIANGULATION GEOM:" + triangulationGeos->toString());
    if (!triangulationGeos)
    {
        VRSA_ERROR("GEOS", "Triangulation has failed");
    }
    //GEOMETRY COLLECTION (OF POLYGONS)
    auto triangulationOGR = geometry::GeometryConverter::createOGRfromGeos(triangulationGeos.get());
    //qDebug()<<gdalwrapper::GeometryTypeConverter::FromOGR(triangulationOGR->getGeometryType());
    auto ds = mCreator->createGDALDatasetFromGeometryCollection(mParams.outputPath, std::move(triangulationOGR), inputLayer);
    ds.reset();
    mCreator->emitLayerReadingRequest(mParams.outputPath);
}
