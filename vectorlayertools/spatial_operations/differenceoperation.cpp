#include "differenceoperation.h"
#include "vector/vectorlayer.h"
#include "geometry/geometryconverter.h"
#include "vectorlayertools/vectorlayercreator.h"

vrsa::vector::DifferenceOperation::DifferenceOperation(VectorLayer *inputLayer, VectorLayer *overlayLayer,
                                          const common::SpatialOperationDTO &dto, VectorLayerCreator *creator)
    : SpatialOperation(dto, creator)
{
    if (inputLayer && overlayLayer)
    setInputLayer(inputLayer);
    setSecondInputLayer(overlayLayer);
}

std::unique_ptr<geos::geom::Geometry> vrsa::vector::DifferenceOperation::execute(const geos::geom::Geometry *geom1,
                                                                                   const geos::geom::Geometry *geom2)
{
    return geom1->difference(geom2);
}

void vrsa::vector::DifferenceOperation::executeOnLayers(VectorLayer *firstLayer, VectorLayer *secondLayer)
{
    if (!firstLayer || !mSecondLayer) return;
    std::vector<gdalwrapper::OgrGeometryPtr> geom;
    std::vector<std::pair<int64_t, int64_t>> fids;
    for (const auto& feature: firstLayer->getFeatures())
    {
        for (const auto& featureOverlay: secondLayer->getFeatures())
        {
            auto featureGeom = feature->getOGRGeometry();
            auto geosFeatureGeom = geometry::GeometryConverter::createGEOSFromOGR(featureGeom);
            auto featureOverlayGeom = featureOverlay->getOGRGeometry();
            auto geosFeatureOverlayGeom = geometry::GeometryConverter::createGEOSFromOGR(featureOverlayGeom);
            auto intersectionGeom = geometry::GeometryConverter::createOGRfromGeos(execute(geosFeatureGeom.get(),
                                                                        geosFeatureOverlayGeom.get()).get());
            if (intersectionGeom)
            {
                geom.emplace_back(std::move(intersectionGeom));
                qDebug()<<"input:" << feature->getFID();
                qDebug()<<"overlay:" << featureOverlay->getFID();
                fids.emplace_back(feature->getFID(), featureOverlay->getFID());
            }
        }
    }
    auto ds = mCreator->createGDALDatasetFromGeometries(mParams.outputPath, std::move(geom), firstLayer);
    if (!ds) return;
    OGRLayer* layer = ds->GetLayer(0);
    if (!layer) mCreator->emitLayerReadingRequest(mParams.outputPath);
    //todo: перенести в vectorlayercreator
    OGRFieldDefn fid1Field("orig_fid", OFTInteger);
    OGRFieldDefn fid2Field("ovrlay_fid", OFTInteger);
    layer->CreateField(&fid1Field);
    layer->CreateField(&fid2Field);
    int featureIndex = 0;
    layer->ResetReading();
    gdalwrapper::OgrFeaturePtr feature;
    while ((feature = gdalwrapper::OgrFeaturePtr(layer->GetNextFeature())) != nullptr)
    {
        if (featureIndex < fids.size())
        {
            const auto& [first, second] = fids.at(featureIndex);
            feature->SetField("orig_fid", static_cast<int>(first));
            feature->SetField("ovrlay_fid", static_cast<int>(second));
            if (layer->SetFeature(feature.get()) != OGRERR_NONE)
                VRSA_LOG_GDAL_ERROR("GEOS", "Error while setting attribute to feature. FID: " +
                                    std::to_string(feature->GetFID()));
        }
        featureIndex++;
    }
    ds.reset();
    mCreator->emitLayerReadingRequest(mParams.outputPath);
}
