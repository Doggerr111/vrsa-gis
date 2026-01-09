#ifndef GDALReader_H
#define GDALReader_H


#include <memory>
#include <vector>
#include <string>
#include "vector/vectorlayer.h"
#include "vector/vectordataset.h"
#include "raster/rasterdataset.h"
#include "common/GisDefines.h"
#include "common/GisTypes.h"
#include <gdal_priv.h>
#include "gdal/gdalresourcehandles.h"
#include "common/logger.h"
#include "common/gisexceptions.h"





namespace vrsa
{
namespace gdalwrapper
{

/**
 * @english
 * @brief GDAL Reader - provides functionality for reading both vector and raster datasets
 * through GDAL library. Supports multiple formats (Shapefile, GeoJSON, GPKG, GeoTIFF, JPEG, etc.)
 * @details The reader can handle both vector data (points, lines, polygons) and raster data (images, DEMs).
 * @endenglish
 * @russian
 * @brief Чтение данных через GDAL - предоставляет функциональность для чтения
 * как векторных, так и растровых наборов данных через библиотеку GDAL.
 * Поддерживает множественные форматы (Shapefile, GeoJSON, GPKG, GeoTIFF, JPEG и др.)
 * @details Может обрабатывать как векторные данные (точки, линии, полигоны),
 * так и растровые данные (изображения, ЦМР).
 * @endrussian
 */
class GDALReader {
public:
    /**
    * @english
    * @brief Default constructor
    * @endenglish
    * @russian
    * @brief Конструктор по умолчанию
    * @endrussian
    */
    GDALReader();
    /**
    * @english
    * @brief Default destructor
    * @endenglish
    * @russian
    * @brief Деструктор по умолчанию
    * @endrussian
    */
    ~GDALReader() = default;


    /**
    * @english
    * @brief Reads complete dataset (vector or raster) from specified source
    * @param source Path to data file or data source identifier
    * @return Unique pointer to Dataset (could be VectorDataset or RasterDataset)
    * @throws GISException if dataset cannot be opened or read
    * @note To determine the polymorph type of returned dataset, call getDatasetType() method
    * @endenglish
    * @russian
    * @brief Читает полный набор данных (векторный или растровый) из указанного источника
    * @param source Путь к файлу данных или идентификатор источника данных
    * @return Умный указатель на Dataset (может быть VectorDataset или RasterDataset)
    * @throws GISException если набор данных не может быть открыт или прочитан
    * @note Для определения полиморфного типа возвращенного набора данных
    * вызовите метод getDatasetType()
    * @endrussian
    */
    std::unique_ptr<vrsa::gdalwrapper::Dataset> readDataset(const std::string& source);


    std::vector<std::unique_ptr<vrsa::raster::RasterChannel>> readChannels(const vrsa::gdalwrapper::GdalDatasetPtr &uPtrDs);
    /**
    * @english
    * @brief Reads all vector layers from opened GDAL dataset
    * @param uPtrDs Smart pointer to opened GDAL dataset
    * @return Vector of unique pointers to VectorLayer objects
    * @throws GISException if layers cannot be read
    * @endenglish
    * @russian
    * @brief Читает все векторные слои из открытого GDAL набора данных
    * @param uPtrDs Умный указатель на открытый GDAL набор данных
    * @return Вектор умных указателей на объекты VectorLayer
    * @throws GISException если слои не могут быть прочитаны
    * @endrussian
    */
    std::vector<std::unique_ptr<vrsa::vector::VectorLayer>> readLayers(const vrsa::gdalwrapper::GdalDatasetPtr &uPtrDs);
    /**
     * @english
     * @brief Converts OGR layer to internal VectorLayer representation
     * @param layer Pointer to OGR layer object
     * @return Unique pointer to converted VectorLayer
     * @throws GISException if conversion fails
     * @endenglish
     * @russian
     * @brief Преобразует OGR слой во внутреннее представление VectorLayer
     * @param layer Указатель на объект OGR слоя
     * @return Умный указатель на преобразованный VectorLayer
     * @throws GISException если преобразование не удалось
     * @endrussian
     */
    std::unique_ptr<vrsa::vector::VectorLayer>  convertOGRLayerToVectorLayer(OGRLayer* layer);


    std::unique_ptr<vrsa::vector::VectorFeature> convertOGRFeatureToVectorFeature(OgrFeaturePtr &ogrFeature);

    vrsa::vector::VectorFeature::AttributeValue convertOGRFieldValue(OgrFeaturePtr &feature,
                                                                   OGRFieldDefn* fieldDef,
                                                                   int fieldIndex);


    static OGRGeometry* getOGRGeometry(const OgrFeaturePtr &feature);

/**
    * @english
    * @brief Detects the type of GDAL dataset (vector, raster, or mixed)
    * @param uPtrDs Smart pointer to GDAL dataset
    * @return DatasetType indicating whether dataset is vector, raster, or mixed
    * @endenglish
    * @russian
    * @brief Определяет тип GDAL набора данных (векторный, растровый или смешанный)
    * @param uPtrDs Умный указатель на GDAL набор данных
    * @return DatasetType, указывающий является ли набор данных векторным, растровым или смешанным
    * @endrussian
    */
    vrsa::common::DatasetType detectDatasetType(const vrsa::gdalwrapper::GdalDatasetPtr &uPtrDs) const;



//    bool canRead(const std::string& source) const;

//    // Информационные методы
//    std::string getFormatName() const;
//    std::vector<std::string> getSupportedExtensions() const;
//    vrsa::common::GeometryType getSupportedGeometryType() const;

//    // Метаданные
//    int getFeatureCount() const;
//    vrsa::common::BoundingBox getBoundingBox() const;

//    std::vector<std::unique_ptr<vrsa::vector::VectorFeature>> readFeatures(int start = 0, int count = -1);



private:
    std::string mSource;
    vrsa::common::BoundingBox mBounding_box_;
    int mFeatureCount;
    vrsa::common::GeometryType mGeometryType;

};
}
}

#endif // GDALReader_H
