#ifndef GDALWRITER_H
#define GDALWRITER_H
#include "ogrsf_frmts.h"
#include <string>
#include <vector>
#include <memory>
#include "gdalresourcehandles.h"
#include "algorithm"
namespace vrsa
{
namespace gdalwrapper
{

//возможности формата
enum class DatasetCapability {
    SingleLayer,     // только один слой (Shapefile, GeoJSON, CSV, GPX, KML, DXF ...)
    MultiLayer       // много слоев (GPKG, PostGIS, SQLIte, Memory)
};

//определяем возможности формата по имени
inline DatasetCapability getFormatCapability(const std::string& format)
{
    //копия строки в нижнем регистре ("ESRI Shapefile" = "esri shapefile")
    std::string lowerFormat = format;
    std::transform(lowerFormat.begin(), lowerFormat.end(), lowerFormat.begin(), ::tolower);
    //ищем подстроку
    //форматы, поддерживающие 1 слой
    if (lowerFormat.find("shapefile") != std::string::npos ||
        lowerFormat.find("shp") != std::string::npos ||
        //lowerFormat.find("geojson") != std::string::npos ||
        lowerFormat.find("csv") != std::string::npos ||
        lowerFormat.find("gpx") != std::string::npos ||
        lowerFormat.find("kml") != std::string::npos ||
        lowerFormat.find("dxf") != std::string::npos )
        return DatasetCapability::SingleLayer;


    return DatasetCapability::MultiLayer;
}

class GDALWriter
{
public:
    GDALWriter();
    ~GDALWriter();

    // Создание нового датасета
    GdalDatasetPtr* createDataset(const std::string& path,
                               const std::string& format = "ESRI Shapefile");

    // Создаёт in-memory датасет с одним слоем
    GdalDatasetPtr* createLayer(
        const std::string& layerName,
        OGRwkbGeometryType geomType,
        const std::vector<OGRFieldType>& fields,
        const std::string& driverName = "ESRI Shapefile"
    );


    // Создание слоя в датасете
    OGRLayer* createLayer(GDALDataset* dataset,
                          const std::string& name,
                          OGRwkbGeometryType geomType,
                          const std::vector<std::pair<std::string, OGRFieldType>>& fields);

    // Запись фич
    bool writeFeature(OGRLayer* layer, OGRFeature* feature);

    // Массовая запись (оптимизированная)
    bool writeFeatures(OGRLayer* layer,
                       const std::vector<OGRFeature*>& features);

    // Транзакции (для поддерживающих форматов)
    bool startTransaction(OGRLayer* layer);
    bool commitTransaction(OGRLayer* layer);
    bool rollbackTransaction(OGRLayer* layer);

    // Настройки
    void setOptions(const std::vector<std::string>& options);

    // Закрытие
    void close(GDALDataset* dataset);

private:
    std::vector<std::string> m_creationOptions;
    bool m_useTransactions = false;
};

}
}

#endif // GDALWRITER_H
