#ifndef GDALVECTORREADER_H
#define GDALVECTORREADER_H


#include <memory>
#include <vector>
#include <string>
#include "vector/vectorlayer.h"
#include "vector/vectordataset.h"
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
 * @brief GDAL vector data reader - provides functionality for reading vector datasets
 * through GDAL/OGR library. Supports multiple formats (Shapefile, GeoJSON, GPKG, etc.)
 * @endenglish
 * @russian
 * @brief Чтение векторных данных через GDAL - предоставляет функциональность для чтения
 * векторных наборов данных через библиотеку GDAL/OGR. Поддерживает множественные форматы
 * (Shapefile, GeoJSON, GPKG и др.)
 * @endrussian
 */
class GDALVectorReader {
public:
    /**
    * @english
    * @brief Default constructor
    * @endenglish
    * @russian
    * @brief Конструктор по умолчанию
    * @endrussian
    */
    GDALVectorReader();
    /**
    * @english
    * @brief Default destructor
    * @endenglish
    * @russian
    * @brief Деструктор по умолчанию
    * @endrussian
    */
    ~GDALVectorReader() = default;


    /**
    * @english
    * @brief Reads complete vector dataset from specified source
    * @param source Path to vector data file or data source identifier
    * @return Unique pointer to VectorDataset containing all layers and metadata
    * @throws GISException if dataset cannot be opened or read
    * @endenglish
    * @russian
    * @brief Читает полный векторный набор данных из указанного источника
    * @param source Путь к файлу векторных данных или идентификатор источника данных
    * @return Умный указатель на VectorDataset, содержащий все слои и метаданные
    * @throws GISException если набор данных не может быть открыт или прочитан
    * @endrussian
    */
    std::unique_ptr<vrsa::vector::VectorDataset> readDataset(const std::string& source);
    /**
    * @english
    * @brief Reads all layers from opened GDAL dataset
    * @param uPtrDs Smart pointer to opened GDAL dataset
    * @return Vector of unique pointers to VectorLayer objects
    * @throws GISException if layers cannot be read
    * @endenglish
    * @russian
    * @brief Читает все слои из открытого GDAL набора данных
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

#endif // GDALVECTORREADER_H
