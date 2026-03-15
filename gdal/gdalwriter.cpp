#include "gdalwriter.h"
#include "common/GisDefines.h"
#include "QFileInfo"
#include "QRegularExpression"
vrsa::gdalwrapper::GdalDatasetPtr vrsa::gdalwrapper::GDALWriter::createLayer(const std::string &layerName,
                OGRwkbGeometryType geomType, OGRSpatialReference* crs, const std::unordered_map<std::string,
                OGRFieldType>&fields, const std::string &driverName, const LayerCreationParams& params) const
{
    if (layerName.empty() || driverName.empty())
        return nullptr;
    auto it = std::find(common::CORE_VECTOR_FORMATS.begin(), common::CORE_VECTOR_FORMATS.end(), driverName);
    if (it==common::CORE_VECTOR_FORMATS.end())
    {
        VRSA_ERROR("GDAL", "Неподдерживаемый формат для создания слоя");
        return nullptr;
    }
    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(driverName.c_str());
    if (!poDriver)
    {
        VRSA_ERROR("GDAL", "Неподдерживаемый формат драйвера GDAL" + driverName);
        return nullptr;
    }


    GdalDatasetPtr poDS;
    poDS = GdalDatasetPtr(poDriver->Create(layerName.c_str(), 0, 0, 0, GDT_Unknown, nullptr)); //createDatasetOptions().get())
    if (!poDS)
    {
        VRSA_LOG_GDAL_ERROR("GDAL", "Failed to create dataset at: " + layerName);
        return nullptr;
    }
    std::string layerGPKG;
    OGRLayer* poLayer;
    if (driverName == "GPKG")
    {
        QFileInfo fileInfo(QString::fromStdString(layerName));

        //без расширения
        QString baseName = fileInfo.baseName();  // "file" из "/path/to/file.gpkg"

        //Удаляем специальные символы (оставляем только буквы, цифры, дефис, подчеркивание)
        QRegularExpression re("[^a-zA-Z0-9_\\-]");
        QString cleanName = baseName;
        cleanName.replace(re, "_");  // заменяем всё лишнее на подчеркивание

        layerGPKG = cleanName.toStdString();
        poLayer = poDS->CreateLayer(layerGPKG.c_str(), crs, geomType, params.toGDALLayerOptions().get());
    }
    else
        poLayer = poDS->CreateLayer(layerName.c_str(), crs, geomType, params.toGDALLayerOptions().get());
    if (!poLayer)
    {
        VRSA_LOG_GDAL_ERROR("GDAL", "Failed to create layer in dataset at: " + layerName);
        return nullptr;
    }
    for (const auto& field: fields)
    {
        OGRFieldDefn fieldDefn(field.first.c_str(), field.second);
        poLayer->CreateField(&fieldDefn);
    }
    return poDS;


}
