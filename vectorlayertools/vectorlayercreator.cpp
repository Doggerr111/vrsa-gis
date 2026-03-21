#include "vectorlayercreator.h"
#include <QDebug>
#include "logger.h"
#include "gdal/gdalwriter.h"
#include "gdal/typeconversions.h"
#include "spatialref/spatialrefdatabase.h"
#include "spatialref/spatialreference.h"
#include "logger.h"
#include "vector/vectorlayer.h"

vrsa::vector::VectorLayerCreator::VectorLayerCreator(QObject *parent)
    : QObject(parent)
{

}

vrsa::gdalwrapper::GdalDatasetPtr vrsa::vector::VectorLayerCreator::createGDALDataset(const common::LayerDefinition &layerDef)
{
    if (!layerDef.isValid())
        return nullptr;
    gdalwrapper::GDALWriter writer;
    common::GeometryType geomType = static_cast<common::GeometryType>(layerDef.geometryType);
    auto spatialRef = spatialref::SpatialReferenceDatabase::instance().createFromName(layerDef.crs);
    VRSA_DEBUG("VECTOR", "spatial ref is valid:" + std::to_string(spatialRef.isValid()));
    std::unordered_map<std::string, OGRFieldType> fieldsRefMap;
    fieldsRefMap.reserve(layerDef.fields.size());
    for (const auto& fieldRef: layerDef.fields)
    {
        std::string fieldName = fieldRef.name;
        OGRFieldType fieldType = gdalwrapper::convertToOGRFieldType(static_cast<common::FieldType>(fieldRef.typeIndex));
        fieldsRefMap[fieldName] = fieldType;
        //        qDebug()<<"creating fieldRef for gdal writer: name:" <<QString::fromStdString(fieldName)
        //               << "fieldType: " << static_cast<common::FieldType>(fieldRef.typeIndex);
    }

    gdalwrapper::LayerCreationParams params;
    params.extraOptions = layerDef.gdalOptions;
    qDebug()<<"internal geom" << static_cast<int>(geomType);
    qDebug()<<"converted" << static_cast<int>(gdalwrapper::ToOGR(geomType));
    auto gdalDsPtr = writer.createLayer(layerDef.filePath, gdalwrapper::ToOGR(geomType), spatialRef.GetOGRSpatialRef(),
                                        fieldsRefMap, layerDef.format, params);
    if (gdalDsPtr)
        VRSA_DEBUG("VECTOR", "Мы смогли!");
    else
    {
        VRSA_DEBUG("VECTOR", "Увы! мы не смогли!(");
    }

    return gdalDsPtr;
}

vrsa::gdalwrapper::GdalDatasetPtr vrsa::vector::VectorLayerCreator::createGDALDataset
(const std::string& path, OGRwkbGeometryType type, OGRSpatialReference* ref)
{
    gdalwrapper::GDALWriter writer;
    auto gdalDsPtr = writer.createLayer(path, type, ref,
                                        {}, "ESRI Shapefile", {});
    if (gdalDsPtr)
        VRSA_DEBUG("VECTOR", "Мы смогли!");
    else
    {
        VRSA_DEBUG("VECTOR", "Увы! мы не смогли!(");
    }

    return gdalDsPtr;
}

vrsa::gdalwrapper::GdalDatasetPtr vrsa::vector::VectorLayerCreator::createGDALDatasetFromGeometries
(const std::string& path, std::vector<gdalwrapper::OgrGeometryPtr> geometries, vector::VectorLayer* origLayer)
{

    auto dsUPtr = createGDALDataset(path, geometries[0]->getGeometryType(), origLayer->getOGRLayer()->GetSpatialRef());
    if (!dsUPtr) return nullptr;
    auto layer = dsUPtr->GetLayer(0);
    if (!layer) return dsUPtr;
    if (geometries.empty()) return dsUPtr;
    for (size_t i = 0; i < geometries.size(); ++i)
    {

        auto feature = gdalwrapper::OgrFeaturePtr(OGRFeature::CreateFeature(layer->GetLayerDefn()));
        if (geometries[i])
        {
            if (feature->SetGeometry(geometries[i].get()->clone()) != OGRERR_NONE)
                VRSA_WARNING("VECTOR", "Error while setting geometry for feature #" + std::to_string(i));
        }

        if (layer->CreateFeature(feature.get()) != OGRERR_NONE)
            VRSA_WARNING("VECTOR", "Error while setting feature #" + std::to_string(i) +
                         " for layer:" + layer->GetName());
    }
    return dsUPtr;
}

vrsa::gdalwrapper::GdalDatasetPtr vrsa::vector::VectorLayerCreator::createGDALDatasetFromGeometryCollection(const std::string &path, gdalwrapper::OgrGeometryPtr collection, VectorLayer *origLayer)
{
    if (collection->getGeometryType() != wkbGeometryCollection) return nullptr;
    auto dsUPtr = createGDALDataset(path, wkbPolygon, origLayer->getOGRLayer()->GetSpatialRef());
    if (!dsUPtr) return nullptr;
    auto layer = dsUPtr->GetLayer(0);
    if (!layer) return dsUPtr;

    auto col = dynamic_cast<OGRGeometryCollection*>(collection.get());
    std::vector<OGRPolygon*> polygons;
    for (int i=0; i<col->getNumGeometries(); ++i)
    {
        OGRGeometry* geom = col->getGeometryRef(i);
        if (!geom) continue;
        switch (geom->getGeometryType())
        {
        case wkbPolygon:
        {
            OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geom);
            if (!polygon) continue;
            polygons.push_back(polygon);
            break;
        }
        default:
            break; //пока так...
        }
    }
    if (polygons.empty()) return dsUPtr;
    for (size_t i = 0; i < polygons.size(); ++i)
    {

        auto feature = gdalwrapper::OgrFeaturePtr(OGRFeature::CreateFeature(layer->GetLayerDefn()));
        if (polygons[i])
        {
            if (feature->SetGeometry(polygons[i]->clone()) != OGRERR_NONE)
                VRSA_WARNING("VECTOR", "Error while setting geometry for feature #" + std::to_string(i));
        }

        if (layer->CreateFeature(feature.get()) != OGRERR_NONE)
            VRSA_WARNING("VECTOR", "Error while setting feature #" + std::to_string(i) +
                         " for layer:" + layer->GetName());
    }
    return dsUPtr;
    }

    //vrsa::gdalwrapper::GdalDatasetPtr vrsa::vector::VectorLayerCreator::createGDALDatasetFromGeometry(const std::string &path, gdalwrapper::OgrGeometryPtr, VectorLayer *origLayer)
    //{

    //}

    std::string vrsa::vector::VectorLayerCreator::getVectorDatasetPath(GDALDataset *dataset)
    {
        if (!dataset) return std::string();

        const char* driverName = dataset->GetDriver()->GetDescription();
        QString path;

        if (EQUAL(driverName, "ESRI Shapefile"))
        {
            const char* description = dataset->GetDescription();
            if (description)
            {
                path = QString::fromUtf8(description);
                //убеждаемся, что есть расширение .shp
                if (!path.endsWith(".shp", Qt::CaseInsensitive))
                    path += ".shp";
            }
        }
        // Для GeoJSON
        else if (EQUAL(driverName, "GeoJSON"))
        {
            const char* description = dataset->GetDescription();
            if (description)
                path = QString::fromUtf8(description);
        }
        else
        {
            char** fileList = dataset->GetFileList();
            if (fileList)
            {
                path = QString::fromUtf8(fileList[0]);
                CSLDestroy(fileList);
            }
        }

        return path.toStdString();
    }

    vrsa::vector::VectorLayerCreator::~VectorLayerCreator() = default;

    void vrsa::vector::VectorLayerCreator::onLayerCreationRequested(const common::LayerDefinition &layerDef)
    {
        VRSA_DEBUG("VECTOR", "WE CAUGHT SIGNAL AND GET LAYER DEF. DRIVER NAME:" + layerDef.format);
        auto dS = createGDALDataset(layerDef);
        if (!dS)
            return;
        auto source = getVectorDatasetPath(dS.get());
        if (!source.empty())
            emit vectorLayerReadingRequested(source);
    }
