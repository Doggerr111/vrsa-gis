#ifndef GDALDELETERS_H
#define GDALDELETERS_H


#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <cpl_conv.h>

#include <common/logger.h>
namespace vrsa {
namespace gdalwrapper {

// ============================================================================
// Основные удалители для GDAL объектов
// ============================================================================

struct GdalDatasetDeleter {
    void operator()(GDALDataset* dataset) const {
        if (dataset) {
#ifdef ENABLE_DEBUG_LOGS
            VRSA_DEBUG("GDAL", "calling GdalDatasetDeleter");
#endif
            GDALClose(dataset);
        }
    }
};

/**
 * @english
 * @brief Deleter for OGRLayer - no explicit close needed (owned by dataset)
 * @endenglish
 * @russian
 * @brief Удалитель для OGRLayer - явное закрытие не требуется (владеется dataset)
 * @endrussian
 */
struct OgrLayerDeleter {
    void operator()(OGRLayer* layer) const {

    }
};

/**
 * @english
 * @brief Deleter for OGRFeature - calls OGRFeature::DestroyFeature()
 * @endenglish
 * @russian
 * @brief Удалитель для OGRFeature - вызывает OGRFeature::DestroyFeature()
 * @endrussian
 */
struct OgrFeatureDeleter {
    void operator()(OGRFeature* feature) const {
        if (feature) {
#ifdef ENABLE_DEBUG_LOGS
            VRSA_DEBUG("GDAL", "calling OgrFeatureDeleter");
#endif
            OGRFeature::DestroyFeature(feature);
        }
    }
};

/**
 * @english
 * @brief Deleter for OGRGeometry - calls OGRGeometryFactory::destroyGeometry()
 * @endenglish
 * @russian
 * @brief Удалитель для OGRGeometry - вызывает OGRGeometryFactory::destroyGeometry()
 * @endrussian
 */
struct OgrGeometryDeleter {
    void operator()(OGRGeometry* geometry) const {
        if (geometry) {
            OGRGeometryFactory::destroyGeometry(geometry);
        }
    }
};

/**
 * @english
 * @brief Deleter for OGRFieldDefn - calls delete
 * @endenglish
 * @russian
 * @brief Удалитель для OGRFieldDefn - вызывает delete
 * @endrussian
 */
struct OgrFieldDefnDeleter {
    void operator()(OGRFieldDefn* fieldDefn) const {
        delete fieldDefn;
    }
};

/**
 * @english
 * @brief Deleter for OGRFeatureDefn - calls OGRFeatureDefn::DestroyFeatureDefn()
 * @endenglish
 * @russian
 * @brief Удалитель для OGRFeatureDefn - вызывает OGRFeatureDefn::DestroyFeatureDefn()
 * @endrussian
 */
struct OgrFeatureDefnDeleter {
    void operator()(OGRFeatureDefn* featureDefn) const {
        if (featureDefn) {
            OGRFeatureDefn::DestroyFeatureDefn(featureDefn);
        }
    }
};

// ============================================================================
// Удалители для Spatial Reference
// ============================================================================

/**
 * @english
 * @brief Deleter for OGRSpatialReference - calls OSRRelease()
 * @endenglish
 * @russian
 * @brief Удалитель для OGRSpatialReference - вызывает OSRRelease()
 * @endrussian
 */
struct OgrSpatialRefDeleter {
    void operator()(OGRSpatialReference* spatialRef) const {
        if (spatialRef) {
            OSRRelease(spatialRef);
        }
    }
};

// ============================================================================
// Удалители для GDAL драйверов и утилит
// ============================================================================

/**
 * @english
 * @brief Deleter for GDALDriver - no explicit deletion needed (managed by GDAL)
 * @endenglish
 * @russian
 * @brief Удалитель для GDALDriver - явное удаление не требуется (управляется GDAL)
 * @endrussian
 */
struct GdalDriverDeleter {
    void operator()(GDALDriver* driver) const {
        // Драйверы управляются GDALDriverManager
        // Не нужно явно удалять
    }
};

///**
// * @english
// * @brief Deleter for GDALRasterBand - no explicit deletion needed (owned by dataset)
// * @endenglish
// * @russian
// * @brief Удалитель для GDALRasterBand - явное удаление не требуется (владеется dataset)
// * @endrussian
// */
//struct GdalRasterBandDeleter {
//    void operator()(GDALRasterBand* band) const {
//        // Band уничтожается вместе с dataset
//    }
//};

// ============================================================================
// Удалители для CPL (Common Portability Library) объектов
// ============================================================================

/**
 * @english
 * @brief Deleter for CPL strings - calls CPLFree()
 * @endenglish
 * @russian
 * @brief Удалитель для CPL строк - вызывает CPLFree()
 * @endrussian
 */
struct CplStringDeleter {
    void operator()(char* str) const {
        if (str) {
            CPLFree(str);
        }
    }
};

/**
 * @english
 * @brief Deleter for CPL string lists - calls CSLDestroy()
 * @endenglish
 * @russian
 * @brief Удалитель для CPL списков строк - вызывает CSLDestroy()
 * @endrussian
 */
struct CplStringListDeleter {
    void operator()(char** stringList) const {
        if (stringList) {
            CSLDestroy(stringList);
        }
    }
};

/**
 * @english
 * @brief Deleter for CPL error handler - restores previous handler
 * @endenglish
 * @russian
 * @brief Удалитель для CPL обработчика ошибок - восстанавливает предыдущий обработчик
 * @endrussian
 */
struct CplErrorHandlerDeleter {
    void operator()(CPLErrorHandler* /* handler */) const {
        // Восстанавливаем стандартный обработчик ошибок
        CPLErrorHandler previousHandler = CPLSetErrorHandler(CPLDefaultErrorHandler);
        (void)previousHandler; // Подавляем warning о неиспользуемой переменной
    }
};



} // namespace gdal
} // namespace vrsa
#endif // GDALDELETERS_H
