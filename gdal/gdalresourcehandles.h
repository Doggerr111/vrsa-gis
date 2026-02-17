#ifndef GDALRESOURCEHANDLES_H
#define GDALRESOURCEHANDLES_H
#include <gdaldeleters.h>


namespace vrsa
{
namespace gdalwrapper
{
//============================================================================
// Умные указатели с предопределенными удалителями
// ===========================================================================

using GdalDatasetPtr = std::unique_ptr<GDALDataset, GdalDatasetDeleter>;
using OgrFeaturePtr = std::unique_ptr<OGRFeature, OgrFeatureDeleter>;
using OgrGeometryPtr = std::unique_ptr<OGRGeometry, OgrGeometryDeleter>;
using OgrSpatialRefPtr = std::unique_ptr<OGRSpatialReference, OgrSpatialRefDeleter>;
using OgrFieldDefnPtr = std::unique_ptr<OGRFieldDefn, OgrFieldDefnDeleter>;
using OgrFeatureDefnPtr = std::unique_ptr<OGRFeatureDefn, OgrFeatureDefnDeleter>;
using CplStringPtr = std::unique_ptr<char, CplStringDeleter>;
using CplStringListPtr = std::unique_ptr<char*, CplStringListDeleter>;

// Shared pointers версии
using GdalDatasetSharedPtr = std::shared_ptr<GDALDataset>;
using OgrFeatureSharedPtr = std::shared_ptr<OGRFeature>;
using OgrGeometrySharedPtr = std::shared_ptr<OGRGeometry>;

// ============================================================================
// Фабричные функции для создания умных указателей
// ============================================================================

/**
 * @english
 * @brief Create GDALDataset smart pointer
 * @endenglish
 * @russian
 * @brief Создать умный указатель на GDALDataset
 * @endrussian
 */
inline GdalDatasetPtr createDataset(const std::string& filename,
                                   unsigned int openFlags = GDAL_OF_VERBOSE_ERROR | GDAL_OF_UPDATE)
{
    return GdalDatasetPtr(
        static_cast<GDALDataset*>(GDALOpenEx(filename.c_str(), openFlags, nullptr, nullptr, nullptr))
    );
}

/**
 * @english
 * @brief Create OGRFeature smart pointer from existing feature
 * @endenglish
 * @russian
 * @brief Создать умный указатель на OGRFeature из существующего feature
 * @endrussian
 */
inline OgrFeaturePtr createFeature(OGRFeature* feature) {
    return OgrFeaturePtr(feature);
}

/**
 * @english
 * @brief Create OGRFeature smart pointer from feature definition
 * @endenglish
 * @russian
 * @brief Создать умный указатель на OGRFeature из определения feature
 * @endrussian
 */
inline OgrFeaturePtr createFeature(OGRFeatureDefn* featureDefn) {
    return OgrFeaturePtr(OGRFeature::CreateFeature(featureDefn));
}

/**
 * @english
 * @brief Create OGRGeometry smart pointer from WKT
 * @endenglish
 * @russian
 * @brief Создать умный указатель на OGRGeometry из WKT
 * @endrussian
 */
inline OgrGeometryPtr createGeometryFromWkt(const std::string& wkt) {
    OGRGeometry* geometry = nullptr;
    OGRGeometryFactory::createFromWkt(wkt.c_str(), nullptr, &geometry);
    return OgrGeometryPtr(geometry);
}

/**
 * @english
 * @brief Create spatial reference from EPSG code
 * @endenglish
 * @russian
 * @brief Создать систему координат из EPSG кода
 * @endrussian
 */
inline OgrSpatialRefPtr createSpatialRef(int epsgCode) {
    OGRSpatialReference* spatialRef = new OGRSpatialReference();
    spatialRef->importFromEPSG(epsgCode);
    return OgrSpatialRefPtr(spatialRef);
}
}
}
#endif // GDALRESOURCEHANDLES_H
