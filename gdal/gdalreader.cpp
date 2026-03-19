#include "gdalreader.h"
#include "vector/vectorlayer.h"
#include "vector/vectorfeature.h"
#include "vector/vectordataset.h"
#include "raster/rasterdataset.h"
#include "raster/webrasterdataset.h"
#include "common/GisDefines.h"
#include <gdal_priv.h>
#include "gdal/gdalresourcehandles.h"
#include "common/logger.h"
#include "common/gisexceptions.h"


vrsa::gdalwrapper::GDALReader::GDALReader() = default;


std::unique_ptr<vrsa::gdalwrapper::Dataset> vrsa::gdalwrapper::GDALReader::readDataset
(const std::string &source, unsigned int flags) const
{
    //создаем уникальный указатель с кастомным удалителем
    auto dS = gdalwrapper::createDataset(source, flags);
    if (!dS)
    {
        //VRSA_LOG_GDAL_ERROR("GDAL", "Failed to open data source: " + source);
        throw common::DataSetOpenException(source);
    }
    switch (detectDatasetType(dS.get()))
    {
    case common::DatasetType::Vector:
    {
        VRSA_DEBUG("GDAL", "Reading Vector Dataset...");
        auto layers = readLayers(dS.get());
        auto vrsaDs = std::make_unique<vector::VectorDataset>(std::move(dS), std::move(layers));
        vrsaDs->SetDatasetType(common::DatasetType::Vector);
        return vrsaDs;

    }
    case common::DatasetType::Raster:
    {
        VRSA_DEBUG("GDAL", "Reading Raster Dataset...");
        auto channels = readChannels(dS.get());
        auto vrsaDs = std::make_unique<raster::RasterDataset>(std::move(dS), std::move(channels));
        vrsaDs->SetDatasetType(common::DatasetType::Raster);
        return vrsaDs;
    }
    default:  //TODO - mixed ds
        VRSA_INFO("GDAL", "Mixed datasets are not supported");
        return nullptr;
    }


}

std::unique_ptr<vrsa::gdalwrapper::Dataset> vrsa::gdalwrapper::GDALReader::readTMSDataset(const std::string &source, unsigned int flags) const
{
    //создаем уникальный указатель с кастомным удалителем
    auto dS = gdalwrapper::createDataset(source, flags);
    if (!dS)
    {
        throw common::DataSetOpenException(source);
    }
    assert (detectDatasetType(dS.get()) == common::DatasetType::Raster);
    VRSA_INFO("GDAL", "Reading Raster TMS Dataset...");
    auto vrsaDs = std::make_unique<raster::WebRasterDataset>(std::move(dS));
    vrsaDs->SetDatasetType(common::DatasetType::Raster);
    return vrsaDs;

}

std::vector<std::unique_ptr<vrsa::raster::RasterChannel>> vrsa::gdalwrapper::GDALReader::readChannels
                                                                        (GDALDataset* ds) const
{
    if (!ds)
        return {};
    std::string source = ds->GetDescription();
    int channelCount = ds->GetRasterCount();
    VRSA_DEBUG("GDAL", "Reading raster channels count:" + std::to_string(channelCount) + " from: " + source);
    if (channelCount == 0)
    {
        VRSA_INFO("GDAL", "No layers found in: " + source);
        return std::vector<std::unique_ptr<raster::RasterChannel>>();
    }
    std::vector<std::unique_ptr<raster::RasterChannel>> channels;
    channels.reserve(channelCount);

    for (int i = 1; i <= channelCount; ++i)
    {
        VRSA_INFO("GDAL", "Reading channel №" + std::to_string(i) + " from: " + source);
        GDALRasterBand* band = ds->GetRasterBand(i);
        if (!band)
        {
            VRSA_ERROR("GDAL", std::string("Warning: Failed to get raster channel") + std::to_string(i) + " from: " + source);
            continue;
        }
        channels.emplace_back(std::make_unique<raster::RasterChannel>(band));

    }



    return channels;

}

std::vector<std::unique_ptr<vrsa::vector::VectorLayer>> vrsa::gdalwrapper::GDALReader::readLayers(GDALDataset *ds) const
{
    std::string source = ds->GetDescription();
    int layerCount = ds->GetLayerCount();
    VRSA_INFO("GDAL", "Reading layers count:" + std::to_string(layerCount) + " from: " + source);

    if (layerCount == 0)
    {
        VRSA_INFO("GDAL", "No layers found in: " + source);
        return std::vector<std::unique_ptr<vector::VectorLayer>>();
    }

    std::vector<std::unique_ptr<vector::VectorLayer>> layers;
    layers.reserve(layerCount);

    // Обрабатываем все слои
    for (int i = 0; i < layerCount; ++i)
    {
        VRSA_INFO("GDAL", "Reading layer №" + std::to_string(i) + " from: " + source);
        OGRLayer* layer = ds->GetLayer(i);
        if (!layer)
        {
            VRSA_ERROR("GDAL", "Failed to read layer №" + std::to_string(i) + " from: " + source);
            continue;
        }

        try
        {
            std::unique_ptr<vector::VectorLayer> vector_layer = convertOGRLayerToVectorLayer(layer);
            if (vector_layer)
                layers.push_back(std::move(vector_layer));
        }
        catch (const std::exception& e)
        {
            VRSA_ERROR("GDAL", "Warning: Failed to convert layer to internal data structures:" +  std::to_string(i) + ": " + e.what());

        }
    }

    if (layers.empty())
    {
        VRSA_INFO("GDAL", "No valid layers could be read from: " + source);
        //throw std::runtime_error("No valid layers could be read from: " + source);
    }

    return layers;
}

std::unique_ptr<vrsa::vector::VectorLayer> vrsa::gdalwrapper::GDALReader::convertOGRLayerToVectorLayer(OGRLayer *layer) const
{
    layer->ResetReading(); //читаем с 1 объекта (на всякий случай)
    OgrFeaturePtr ogrFeature;
    std::vector<std::unique_ptr<vector::VectorFeature>> mFeatures;
    //конвертируем во внутренние фичи
    while ((ogrFeature = OgrFeaturePtr(layer->GetNextFeature())) != nullptr)
        mFeatures.emplace_back(std::make_unique<vector::VectorFeature>(std::move(ogrFeature), layer));


    auto vL = std::make_unique<vector::VectorLayer>(layer);
    vL->setFeatures(std::move(mFeatures));
    return vL;

}

vrsa::common::DatasetType vrsa::gdalwrapper::GDALReader::detectDatasetType(GDALDataset* GdalDs) const
{
    using namespace common;
    if (!GdalDs) return DatasetType::Unknown;

    int layerCount = GdalDs->GetLayerCount();
    int bandCount = GdalDs->GetRasterCount();
    //вообще то тут надо проверять сами форматы конечно...
    if (layerCount > 0 && bandCount > 0)
        return DatasetType::Mixed;  //форматы вроде GeoPackage
    else if (layerCount > 0)
        return DatasetType::Vector;
    else if (bandCount > 0)
        return DatasetType::Raster;
    else
        return DatasetType::Empty;

}

bool vrsa::gdalwrapper::GDALReader::validatePostGisConnectionString(const std::string &connectionString)
{
    if (connectionString.empty()) return false;
    return gdalwrapper::createDataset(connectionString, GDAL_OF_VECTOR | GDAL_OF_UPDATE) ? true : false;
}

bool vrsa::gdalwrapper::GDALReader::validateTMSConnection(const std::string &xml)
{
    if (xml.empty()) return false;
    bool hasServerUrl = xml.find("<ServerUrl>") != std::string::npos;
    bool hasServiceName = xml.find("name=\"TMS\"") != std::string::npos ||
            xml.find("<Service name=\"TMS\"") != std::string::npos;
    bool hasDataWindow = xml.find("<DataWindow>") != std::string::npos;

    if (!hasServerUrl) {
        VRSA_ERROR("TMS", "Missing <ServerUrl> in XML config");
        return false;
    }

    if (!hasServiceName) {
        VRSA_WARNING("TMS", "Service name not explicitly set to TMS, will attempt anyway");
    }

    if (!hasDataWindow) {
        VRSA_WARNING("TMS", "Missing <DataWindow>, GDAL will use defaults");
    }

    // Проверяем базовую структуру XML
    if (xml.find("<GDAL_WMS>") == std::string::npos) {
        VRSA_ERROR("TMS", "Root element <GDAL_WMS> not found");
        return false;
    }

    if (xml.find("</GDAL_WMS>") == std::string::npos) {
        VRSA_ERROR("TMS", "Closing </GDAL_WMS> not found");
        return false;
    }
    return gdalwrapper::createDataset(xml, GDAL_OF_RASTER | GDAL_OF_READONLY) ? true : false;
}






