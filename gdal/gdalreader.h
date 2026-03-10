#ifndef GDALReader_H
#define GDALReader_H


#include <memory>
#include <vector>
#include <string>
#include <gdal_priv.h>

namespace vrsa
{
namespace common
{
enum class DatasetType : int;
}
namespace vector
{
    class VectorDataset;
    class VectorLayer;
    class VectorFeature;
}
namespace raster
{
    class RasterChannel;
}
namespace gdalwrapper
{
    class Dataset;

/**
 * @english
 * @brief GDAL Reader - provides functionality to convert GDAL data structures to internal ones
 * @details The reader can handle both vector and raster data.
 * @endenglish
 * @russian
 * @brief GDAL Reader - конвертирует структуры GDAL во внутренний формат
 * @details Поддерживает векторные и растровые данные.
 * @endrussian
 */
class GDALReader {
public:

    GDALReader();
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


    std::vector<std::unique_ptr<vrsa::raster::RasterChannel>> readChannels(GDALDataset* ds);
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
    std::vector<std::unique_ptr<vrsa::vector::VectorLayer>> readLayers(GDALDataset* ds);
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


    std::unique_ptr<vrsa::vector::VectorFeature> convertOGRFeatureToVectorFeature(OGRFeature* rawOgrFeature, OGRLayer* layer);

//    vrsa::vector::VectorFeature::AttributeValue convertOGRFieldValue(OGRFeature *feature,
//                                                                   OGRFieldDefn* fieldDef,
//                                                                   int fieldIndex);




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
    vrsa::common::DatasetType detectDatasetType(GDALDataset *GdalDs) const;



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
    //    std::string mSource;
    //    vrsa::common::BoundingBox mBounding_box;
    //    int mFeatureCount;
    //    vrsa::common::GeometryType mGeometryType;

};
}
}

#endif // GDALReader_H
