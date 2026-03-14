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

struct LayerCreationParams
{
    //общие параметры
    std::string encoding = "UTF-8";
    bool createSpatialIndex = false;
    bool overwrite = false;

    //специфичные для формата
    std::string fidColumn = "fid";
    std::string geometryColumn = "geom";
    int coordinatePrecision = 6;
    bool writeBbox = false;

    //дополнительные опции (ключ-значение)
    std::unordered_map<std::string, std::string> extraOptions;

    //конвертация в GDAL options
    CplStringListPtr toGDALLayerOptions() const
    {
        char** options = nullptr;

        // Базовые опции
        if (!encoding.empty())
            options = CSLSetNameValue(options, "ENCODING", encoding.c_str());

        if (createSpatialIndex)
            options = CSLSetNameValue(options, "SPATIAL_INDEX", "YES");

        if (overwrite)
            options = CSLSetNameValue(options, "OVERWRITE", "YES");

        if (!fidColumn.empty())
            options = CSLSetNameValue(options, "FID", fidColumn.c_str());

        if (!geometryColumn.empty())
            options = CSLSetNameValue(options, "GEOMETRY_COLUMN", geometryColumn.c_str());

        if (coordinatePrecision > 0)
            options = CSLSetNameValue(options, "COORDINATE_PRECISION",
                                      std::to_string(coordinatePrecision).c_str());

        if (writeBbox)
            options = CSLSetNameValue(options, "WRITE_BBOX", "YES");

        //дополнительные опции
        for (const auto& [key, value] : extraOptions)
        {
            options = CSLSetNameValue(options, key.c_str(), value.c_str());
        }

        return CplStringListPtr(options);
    }
};


/**
 * @english
 * @brief GDAL Writer - creates vector layers using GDAL API
 *
 * Provides interface for creating vector datasets and layers
 * in common formats supported by GDAL (shp, gpkg, geojson). Handles:
 * - Dataset creation with format-specific options
 * - Layer creation with geometry type and fields
 *
 * @note Caller takes ownership of returned GdalDatasetPtr
 *
 * @russian
 * @brief GDAL Writer - создает векторные слои через GDAL API
 *
 * Предоставляет интерфейс для создания векторных датасетов и слоев
 * в распространенных форматах, поддерживаемых GDAL (shp, gpkg, geojson). Обеспечивает:
 * - Создание датасета с опциями формата
 * - Создание слоя с типом геометрии и полями
 *
 * @note Вызывающий код получает владение через GdalDatasetPtr
 */
class GDALWriter
{
public:
    GDALWriter() = default;
    /**
     * @english
     * @brief Creates a new vector layer in a new dataset
     *
     * Creates a new dataset with one layer using specified format.
     * The dataset and layer are configured according to provided parameters.
     *
     * @param layerName Name of the layer to create
     * @param geomType Geometry type for the layer (wkbPoint, wkbLineString, etc.)
     * @param crs Spatial reference system (nullptr for unknown/not set)
     * @param fields Map of field names to OGR field types
     * @param driverName GDAL driver name (default: "ESRI Shapefile")
     * @param params Additional layer creation parameters (encoding, spatial index, etc.)
     *
     * @return GdalDatasetPtr smart pointer to created dataset (nullptr on error)
     * @note Caller takes ownership of the returned dataset
     *
     * @russian
     * @brief Создает новый векторный слой в новом датасете
     *
     * Создает новый датасет с одним слоем в указанном формате.
     * Датасет и слой настраиваются согласно переданным параметрам.
     *
     * @param layerName Имя создаваемого слоя
     * @param geomType Тип геометрии слоя (wkbPoint, wkbLineString и т.д.)
     * @param crs Система координат (nullptr если неизвестна/не задана)
     * @param fields Мапа "имя поля" -> "OGR тип поля"
     * @param driverName Имя GDAL драйвера (по умолчанию: "ESRI Shapefile")
     * @param params Дополнительные параметры создания слоя (кодировка, индекс и т.д.)
     *
     * @return GdalDatasetPtr умный указатель на созданный датасет (nullptr при ошибке)
     * @note Вызывающий код получает владение возвращаемым датасетом
     */
    GdalDatasetPtr createLayer(
            const std::string& layerName,
            OGRwkbGeometryType geomType,
            OGRSpatialReference* crs,
            const std::unordered_map<std::string, OGRFieldType>& fields,
            const std::string& driverName = "ESRI Shapefile",
            const LayerCreationParams& params = LayerCreationParams()) const;


    CplStringListPtr createDatasetOptions() const
    {
        char** raw = nullptr;
        //например для shp
        raw = CSLSetNameValue(raw, "ENCODING", "UTF-8");
        //для ускорения в gpkg, geojson
        raw = CSLSetNameValue(raw, "SPATIAL_INDEX", "YES");
        raw = CSLSetNameValue(raw, "WRITE_BBOX", "YES");
        return CplStringListPtr(raw);
    }

};

}
}

#endif // GDALWRITER_H
