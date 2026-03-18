#include "vectorlayercreator.h"
#include <QDebug>
#include "logger.h"
#include "gdal/gdalwriter.h"
#include "gdal/typeconversions.h"
#include "spatialref/spatialrefdatabase.h"
#include "spatialref/spatialreference.h"

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
