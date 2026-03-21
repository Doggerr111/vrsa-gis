#ifndef VECTORLAYERCREATOR_H
#define VECTORLAYERCREATOR_H
#include <QObject>
#include "common/layercreationparams.h"
#include "gdalresourcehandles.h"
namespace vrsa
{
namespace vector
{

class VectorLayer;
/**
 * @english
 * @brief Mediator class that organizes vector layer creation from UI to final data structures
 *
 * VectorLayerCreator acts as a mediator that:
 * 1. Receives layer description from UI (via LayerDefinition DTO)
 * 2. Uses GDALWriter to create physical layer via GDAL API
 * 3. Uses GDALReader to convert GDAL structures into internal data structures (VectorDataset, VectorLayer, VectorFeature)
 * @russian
 * @brief Класс-посредник, управляющий созданием векторного слоя от UI до внутренних структур
 *
 * VectorLayerCreator выступает в роли фасада/медиатора, который:
 * 1. Получает описание слоя из UI (через DTO LayerDefinition)
 * 2. Использует GDALWriter для создания физического слоя через GDAL API
 * 3. Использует GDALReader для конвертации структур GDAL во внутренние (VectorDataset, VectorLayer, VectorFeature)
 */
class VectorLayerCreator: public QObject
{
    Q_OBJECT
public:
    VectorLayerCreator(QObject* parent = nullptr);
    ~VectorLayerCreator();
public:
    gdalwrapper::GdalDatasetPtr createGDALDataset(const common::LayerDefinition& layerDef);
    //создаем датасет с такой же системой координат как у origLayer
    gdalwrapper::GdalDatasetPtr createGDALDataset(const std::string& path, OGRwkbGeometryType type
                                                  = wkbUnknown, OGRSpatialReference* ref = nullptr);
    gdalwrapper::GdalDatasetPtr createGDALDatasetFromGeometries(const std::string& path, std::vector<gdalwrapper::OgrGeometryPtr> vec,
                                                                vector::VectorLayer* origLayer = nullptr);
//    gdalwrapper::GdalDatasetPtr createGDALDatasetFromGeometry(const std::string& path,
//                                                              gdalwrapper::OgrGeometryPtr,
//                                                              vector::VectorLayer* origLayer = nullptr);
    //ПОКА ЧТО ИСПОЛЬЗУЕТСЯ ТОЛЬКО ДЛЯ ТРИАНГУЛЯЦИЙ
    gdalwrapper::GdalDatasetPtr createGDALDatasetFromGeometryCollection(const std::string& path,
                                                                        gdalwrapper::OgrGeometryPtr collection,
                                                                        vector::VectorLayer* origLayer = nullptr);
    std::string getVectorDatasetPath(GDALDataset* dataset);

    void emitLayerReadingRequest(const std::string& src)
    {
        emit vectorLayerReadingRequested(src);
    }

public slots:
    void onLayerCreationRequested(const common::LayerDefinition& layerDef);
signals:
    void vectorLayerReadingRequested(const std::string& src);
};

}
}
#endif // VECTORLAYERCREATOR_H
