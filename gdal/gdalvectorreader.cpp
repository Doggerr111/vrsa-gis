#include "gdalvectorreader.h"


using namespace vrsa::vector;
vrsa::gdalwrapper::GDALVectorReader::GDALVectorReader()
{

}

std::unique_ptr<VectorDataset> vrsa::gdalwrapper::GDALVectorReader::readDataset(const std::string &source)
{
    auto dS = vrsa::gdalwrapper::createDataset(source, GDAL_OF_VECTOR | GDAL_OF_READONLY);
    if (!dS)
    {
        throw vrsa::common::DataSetOpenException(source);
        VRSA_LOG_GDAL_ERROR("GDAL", "Failed to open data source: " + source);

    }

    auto layers = readLayers(dS);
    return std::make_unique<vrsa::vector::VectorDataset>(std::move(dS), source, std::move(layers));
    //return vDs;
}

std::vector<std::unique_ptr<vrsa::vector::VectorLayer>> vrsa::gdalwrapper::GDALVectorReader::readLayers
                                           (const vrsa::gdalwrapper::GdalDatasetPtr &uPtrDs)
{
    std::string source = uPtrDs->GetDescription();
    int layerCount = uPtrDs->GetLayerCount();
    VRSA_DEBUG("GDAL", "Reading layers count:" + std::to_string(layerCount) + " from: " + source);

    if (layerCount == 0)
    {
        return std::vector<std::unique_ptr<vrsa::vector::VectorLayer>>();
        VRSA_INFO("GDAL", "No layers found in: " + source);
    }

    std::vector<std::unique_ptr<VectorLayer>> layers;
    layers.reserve(layerCount);

    // Обрабатываем все слои
    for (int i = 0; i < layerCount; ++i)
    {
        VRSA_INFO("GDAL", "Reading layer №" + std::to_string(i) + " from: " + source);
        OGRLayer* layer = uPtrDs->GetLayer(i);
        if (!layer)
        {
            VRSA_ERROR("GDAL", std::string("Warning: Failed to get layer ") + std::to_string(i) + " from: " + source);
            continue;
        }

        try
        {
            auto vector_layer = convertOGRLayerToVectorLayer(layer);
            if (vector_layer)
                layers.push_back(std::move(vector_layer));
        }
        catch (const std::exception& e)
        {
            VRSA_ERROR("GDAL", "Warning: Failed to convert layer " +  std::to_string(i) + ": " + e.what());

        }
    }

    if (layers.empty()) {
        throw std::runtime_error("No valid layers could be read from: " + source);
    }

    return layers;
}

std::unique_ptr<vrsa::vector::VectorLayer> vrsa::gdalwrapper::GDALVectorReader::convertOGRLayerToVectorLayer(OGRLayer *layer)
{
    layer->ResetReading();
    //OGRFeature* ogrFeature;
    OgrFeaturePtr ogrFeature;
    int featureCount = 0;
    std::vector<std::unique_ptr<vrsa::vector::VectorFeature>> mFeatures;
    while ((ogrFeature = OgrFeaturePtr(layer->GetNextFeature())) != nullptr) {
           auto vectorFeature = std::make_unique<VectorFeature>(std::move(ogrFeature));
           if (vectorFeature) {
               mFeatures.push_back(std::move(vectorFeature));
               featureCount++;
           }
     //      OGRFeature::DestroyFeature(ogrFeature);
       }

    auto vL = std::make_unique<vrsa::vector::VectorLayer>(layer);
    vL->setFeatures(std::move(mFeatures));
    return vL;

}

std::unique_ptr<vrsa::vector::VectorFeature> vrsa::gdalwrapper::GDALVectorReader::convertOGRFeatureToVectorFeature(OgrFeaturePtr &ogrFeature)
{
    if (!ogrFeature) return nullptr;

       auto vectorFeature = std::make_unique<vrsa::vector::VectorFeature>(std::move(ogrFeature));

       // Геометрия
       OGRGeometry* ogrGeometry = ogrFeature->GetGeometryRef();
       if (ogrGeometry) {
           vectorFeature->setGeometry(vrsa::gdalwrapper::OgrGeometryPtr(ogrGeometry->clone()));
       }

       // ID/имя
       vectorFeature->setName(std::to_string(ogrFeature->GetFID()));

       // Атрибуты
       OGRFeatureDefn* featureDefn = ogrFeature->GetDefnRef();
       if (featureDefn) {
           for (int i = 0; i < featureDefn->GetFieldCount(); ++i) {
               OGRFieldDefn* fieldDef = featureDefn->GetFieldDefn(i);
               std::string fieldName = fieldDef->GetNameRef();

               // Конвертируем значение поля
               vrsa::vector::VectorFeature::AttributeValue value = convertOGRFieldValue(ogrFeature, fieldDef, i);
               vectorFeature->setAttribute(fieldName, value);
           }
       }

       vectorFeature->setVisible(true);
       return vectorFeature;
}

vrsa::vector::VectorFeature::AttributeValue vrsa::gdalwrapper::GDALVectorReader::convertOGRFieldValue(OgrFeaturePtr &ogrFeature, OGRFieldDefn *fieldDef, int fieldIndex)
{
    if (!ogrFeature->IsFieldSet(fieldIndex)) {
        return nullptr; // NULL значение
    }

    switch (fieldDef->GetType()) {
        case OFTInteger:
            return ogrFeature->GetFieldAsInteger(fieldIndex);

        case OFTInteger64:
            // Приводим к int (или можно использовать int64_t в variant)
            return static_cast<int>(ogrFeature->GetFieldAsInteger64(fieldIndex));

        case OFTReal:
            return ogrFeature->GetFieldAsDouble(fieldIndex);

        case OFTString:
            return std::string(ogrFeature->GetFieldAsString(fieldIndex));

        case OFTDate:
        case OFTTime:
        case OFTDateTime:
            // Даты/время конвертируем в строку
            return std::string(ogrFeature->GetFieldAsString(fieldIndex));

//        case OFTIntegerList:
//        case OFTInteger64List:
//        case OFTRealList:
//        case OFTStringList:
//            // Списки конвертируем в строку
//            return convertOGRListToString(ogrFeature, fieldIndex);

        default:
            // Для неподдерживаемых типов возвращаем строковое представление
            return std::string(ogrFeature->GetFieldAsString(fieldIndex));
    }
}


