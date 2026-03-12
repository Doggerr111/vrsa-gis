#include "gdalwriter.h"
#include "common/GisDefines.h"


vrsa::gdalwrapper::GdalDatasetPtr vrsa::gdalwrapper::GDALWriter::createLayer(const std::string &layerName,
                OGRwkbGeometryType geomType, OGRSpatialReference* crs, const std::unordered_map<std::string,
                OGRFieldType>&fields, const std::string &driverName, const LayerCreationParams& params) const
{
    if (layerName.empty() || driverName.empty())
        return nullptr;
    auto it = std::find(common::CORE_VECTOR_FORMATS.begin(), common::CORE_VECTOR_FORMATS.end(), driverName);
    if (it==common::CORE_VECTOR_FORMATS.end())
    {
        VRSA_ERROR("GDALWriter", "Неподдерживаемый формат для создания слоя");
        return nullptr;
    }
    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(driverName.c_str());
    if (!poDriver)
    {
        VRSA_ERROR("GDALWriter", "Неподдерживаемый формат драйвера GDAL" + driverName);
        return nullptr;
    }


    GdalDatasetPtr poDS;
    poDS = GdalDatasetPtr(poDriver->Create(layerName.c_str(), 0, 0, 0, GDT_Unknown, createDatasetOptions().get()));
    if (!poDS)
    {
        VRSA_LOG_GDAL_ERROR("GDALWriter", "Failed to create dataset at: " + layerName);
        return nullptr;
    }

    OGRLayer* poLayer = poDS->CreateLayer(layerName.c_str(), crs, geomType, params.toGDALLayerOptions().get());
    if (!poLayer)
    {
        VRSA_LOG_GDAL_ERROR("GDALWriter", "Failed to create layer in dataset at: " + layerName);
        return nullptr;
    }
    for (const auto& field: fields)
    {
        OGRFieldDefn fieldDefn(field.first.c_str(), field.second);
        poLayer->CreateField(&fieldDefn);
    }
    return poDS;


}
