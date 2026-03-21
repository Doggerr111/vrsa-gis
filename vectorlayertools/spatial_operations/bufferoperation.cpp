#include "bufferoperation.h"
#include "vector/vectorlayer.h"
#include "geometry/geometryconverter.h"
#include "vectorlayertools/vectorlayercreator.h"

vrsa::vector::BufferOperation::BufferOperation(VectorLayer *inputLayer, const common::SpatialOperationDTO &dto,
                                               VectorLayerCreator *creator)
    : SpatialOperation(dto, creator)
{
    if (inputLayer) setInputLayer(inputLayer);
}

std::unique_ptr<geos::geom::Geometry> vrsa::vector::BufferOperation::execute(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2)
{
    return geom1->buffer(mParams.distance);
}

void vrsa::vector::BufferOperation::executeOnLayer(VectorLayer *inputLayer)
{
    std::vector<gdalwrapper::OgrGeometryPtr> geom;
    for (const auto& feature: inputLayer->getFeatures())
    {
        auto featureGeom = feature->getOGRGeometry();
        auto geosFeatureGeom = geometry::GeometryConverter::createGEOSFromOGR(featureGeom);
        geom.emplace_back(geometry::GeometryConverter::createOGRfromGeos(execute(geosFeatureGeom.get(), nullptr).get()));
    }
    auto ds = mCreator->createGDALDatasetFromGeometries(mParams.outputPath, std::move(geom), inputLayer);
    ds.reset();
    mCreator->emitLayerReadingRequest(mParams.outputPath);

    //геос не дружит с многопоточностью (хотя скорее я не дружу) :c
//    QtConcurrent::run([=]() mutable {
//
//        for (const auto& feature: inputLayer->getFeatures())
//        {
//            auto featureGeom = feature->getOGRGeometry();
//            if (!featureGeom) continue;

//            auto geosFeatureGeom = geometry::GeometryConverter::createGEOSFromOGR(featureGeom);
//            if (!geosFeatureGeom) continue;

//            auto resultGeom = execute(geosFeatureGeom.get(), nullptr);
//            if (!resultGeom) continue;

//            auto ogrGeom = geometry::GeometryConverter::createOGRfromGeos(resultGeom.get());
//            if (!ogrGeom) continue;

//            geom.emplace_back(std::move(ogrGeom));
//        }

//
//        QMetaObject::invokeMethod(this, [=, geom = std::move(geom)]() mutable{
//            auto ds = mCreator->createGDALDatasetFromGeometries(mParams.outputPath, std::move(geom), inputLayer);
//            ds.reset();
//            mCreator->emitLayerReadingRequest(mParams.outputPath);
//        });
//    });

}
