#include "gdalreader.h"
#include "vector/vectorlayer.h"
#include "vector/vectordataset.h"
#include "raster/rasterdataset.h"
#include "common/GisDefines.h"
#include <gdal_priv.h>
#include "gdal/gdalresourcehandles.h"
#include "common/logger.h"
#include "common/gisexceptions.h"

vrsa::gdalwrapper::GDALReader::GDALReader() = default;


std::unique_ptr<vrsa::gdalwrapper::Dataset> vrsa::gdalwrapper::GDALReader::readDataset(const std::string &source)
{
    //создаем уникальный указатель с кастомным удалителем
    auto dS = gdalwrapper::createDataset(source, GDAL_OF_VECTOR | GDAL_OF_UPDATE);
    if (!dS)
    {
        throw common::DataSetOpenException(source);
        VRSA_LOG_GDAL_ERROR("GDAL", "Failed to open data source: " + source);
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
        VRSA_DEBUG("GDAL", "Reading Raster Dataset");
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
//TODO REFACTOR THIS....
std::vector<std::unique_ptr<vrsa::raster::RasterChannel> > vrsa::gdalwrapper::GDALReader::readChannels
                                                                        (GDALDataset* ds)
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
        channels.push_back(std::make_unique<raster::RasterChannel>(band));

    }



    return channels;

}

std::vector<std::unique_ptr<vrsa::vector::VectorLayer>> vrsa::gdalwrapper::GDALReader::readLayers(GDALDataset *ds)
{
    std::string source = ds->GetDescription();
    int layerCount = ds->GetLayerCount();
    VRSA_DEBUG("GDAL", "Reading layers count:" + std::to_string(layerCount) + " from: " + source);

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
            VRSA_ERROR("GDAL", "Warning: Failed to convert layer " +  std::to_string(i) + ": " + e.what());

        }
    }

    if (layers.empty())
    {
        VRSA_DEBUG("GDAL", "No valid layers could be read from: " + source);
        //throw std::runtime_error("No valid layers could be read from: " + source);
    }

    return layers;
}

std::unique_ptr<vrsa::vector::VectorLayer> vrsa::gdalwrapper::GDALReader::convertOGRLayerToVectorLayer(OGRLayer *layer)
{
    layer->ResetReading(); //читаем с 1 объекта (на всякий случай)
    OGRFeature* rawOGRFeature;
    int featureCount = 0;
    std::vector<std::unique_ptr<vector::VectorFeature>> mFeatures;
    //конвертируем во внутренние фичи
    while ((rawOGRFeature = layer->GetNextFeature()) != nullptr)
    {
        mFeatures.push_back(std::make_unique<vector::VectorFeature>(OgrFeaturePtr(rawOGRFeature), layer));
        featureCount++;
    }

    auto vL = std::make_unique<vector::VectorLayer>(layer);
    vL->setFeatures(std::move(mFeatures));
    return vL;

}

std::unique_ptr<vrsa::vector::VectorFeature> vrsa::gdalwrapper::GDALReader::convertOGRFeatureToVectorFeature
(OGRFeature *rawOgrFeature, OGRLayer *layer)
{
    if (!rawOgrFeature)
        return nullptr;
    // передаем фичу с кастомным удалителем
    return std::make_unique<vector::VectorFeature>(OgrFeaturePtr(rawOgrFeature), layer);


    //TODO перенести в конструктор фичи!!!!!!!!!!!!!!!!!

    //    auto ogrFeature = vectorFeature->getOGRFeature();
    //    vectorFeature->setName(std::to_string(ogrFeature->GetFID()));
    //    // Атрибуты
    //    OGRFeatureDefn* featureDefn = ogrFeature->GetDefnRef();
    //    if (featureDefn)
    //    {
    //        for (int i = 0; i < featureDefn->GetFieldCount(); ++i)
    //        {
    //            OGRFieldDefn* fieldDef = featureDefn->GetFieldDefn(i);
    //            std::string fieldName = fieldDef->GetNameRef();

    //            // Конвертируем значение поля
    //            vector::VectorFeature::AttributeValue value = convertOGRFieldValue(ogrFeature, fieldDef, i);
    //            vectorFeature->setAttribute(fieldName, value);
    //        }
    //    }

    //    vectorFeature->setVisible(true);
    //    return vectorFeature;
}

//vrsa::vector::VectorFeature::AttributeValue vrsa::gdalwrapper::GDALReader::convertOGRFieldValue(OGRFeature* ogrFeature, OGRFieldDefn *fieldDef, int fieldIndex)
//{
//    if (!ogrFeature->IsFieldSet(fieldIndex)) {
//        return nullptr; // NULL значение
//    }

//    switch (fieldDef->GetType()) {
//    case OFTInteger:
//        return ogrFeature->GetFieldAsInteger(fieldIndex);

//    case OFTInteger64:
//        // Приводим к int (или можно использовать int64_t в variant)
//        return static_cast<int>(ogrFeature->GetFieldAsInteger64(fieldIndex));

//    case OFTReal:
//        return ogrFeature->GetFieldAsDouble(fieldIndex);

//    case OFTString:
//        return std::string(ogrFeature->GetFieldAsString(fieldIndex));

//    case OFTDate:
//    case OFTTime:
//    case OFTDateTime:
//        // Даты/время конвертируем в строку
//        return std::string(ogrFeature->GetFieldAsString(fieldIndex));

//        //        case OFTIntegerList:
//        //        case OFTInteger64List:
//        //        case OFTRealList:
//        //        case OFTStringList:
//        //            // Списки конвертируем в строку
//        //            return convertOGRListToString(ogrFeature, fieldIndex);

//    default:
//        // Для неподдерживаемых типов возвращаем строковое представление
//        return std::string(ogrFeature->GetFieldAsString(fieldIndex));
//    }
//}


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


