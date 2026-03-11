#include "vectorlayer.h"
#include "gdal/ogrconverter.h"
#include "gdal/typeconversions.h"
#include "gdal/geometrytypeconverter.h"
#include "graphics/vectorfeaturestyle.h"
vrsa::vector::VectorLayer::VectorLayer(OGRLayer* ogrLayer)
    : mOGRLayer{ogrLayer},
      mFeatures{},
      mZValue{-1},
      mIsSelected{false},
      geomType{common::GeometryType::Unknown},
      mStyle{},
      mStyles{}
{

}

vrsa::vector::VectorLayer::~VectorLayer() = default;


QRectF vrsa::vector::VectorLayer::getBoundingBox() const
{
    OGREnvelope envelope;
    auto er=mOGRLayer->GetExtent(&envelope, true);
    if (er!=OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("VectorLayer", "Can't get bounding box from OGRLayer");
        return QRectF();
    }
    double minX = envelope.MinX;
    double minY = envelope.MinY;
    double maxX = envelope.MaxX;
    double maxY = envelope.MaxY;
    return QRectF(QPointF(minX, minY), QSizeF(maxX - minX, maxY - minY));
}

OGREnvelope vrsa::vector::VectorLayer::getOGRBoundingBox() const
{
    OGREnvelope envelope;
    auto er = mOGRLayer->GetExtent(&envelope, true);
    if (er!=OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("VectorLayer", "Can't get bounding box from OGRLayer");
        return {};
    }
    return envelope;
}

std::vector<vrsa::vector::VectorFeature *> vrsa::vector::VectorLayer::getFeatures()
{
    std::vector<VectorFeature*> ftrPtrs;
    ftrPtrs.reserve(mFeatures.size());
    for (const auto& feature : mFeatures)
        ftrPtrs.emplace_back(feature.get());
    return ftrPtrs;
}


vrsa::vector::VectorFeature* vrsa::vector::VectorLayer::getFeatureAt(size_t idx) noexcept
{
    return (idx < mFeatures.size()) ? mFeatures[idx].get() : nullptr;
}

const vrsa::vector::VectorFeature *vrsa::vector::VectorLayer::getFeatureAt(size_t idx) const noexcept
{
    return (idx < mFeatures.size()) ? mFeatures[idx].get() : nullptr;
}

bool vrsa::vector::VectorLayer::addFeature(std::unique_ptr<VectorFeature> feature)
{
    auto ogrF = feature->getOGRFeature();
    if (!ogrF)
    {
        VRSA_ERROR("VectorLayer", "Vector feature on input has nullptr OGRfeature");
        return false;
    }
    auto err = mOGRLayer->CreateFeature(ogrF);

    if (err == OGRERR_NONE)
    {
        VRSA_DEBUG("VectorLayer", "Feature added! FID:" + std::to_string(ogrF->GetFID()));
        mFeatures.push_back(std::move(feature));
        mOGRLayer->SyncToDisk();
        return true;
    }
    else
    {
        VRSA_LOG_GDAL_ERROR("VectorLayer", "Can't add vector feature to layer");
        return false;
    }
}

bool vrsa::vector::VectorLayer::deleteFeature(int64_t fid)
{
    if (!mOGRLayer)
    {
        VRSA_ERROR("VectorLayer", "Can't delete feature:" + std::to_string(fid)+ " VectorLayer has nullptr OGRLayer");
        return false;
    }

    OGRErr err = mOGRLayer->DeleteFeature(fid);
    if (err != OGRERR_NONE)
    {
        VRSA_ERROR("VectorLayer", "Can't delete feature:" + std::to_string(fid)+ " OGR error:" + std::to_string(err));
        return false;
    }
    auto it = std::remove_if(mFeatures.begin(), mFeatures.end(),
                             [fid](const auto& f) {
        auto ogrF = f->getOGRFeature();
        return ogrF && ogrF->GetFID() == fid;
    });
    if (it != mFeatures.end())
    {
        mFeatures.erase(it, mFeatures.end());
        emit featureRemoved(fid);
    }
    mOGRLayer->SyncToDisk();
    return true;
}

bool vrsa::vector::VectorLayer::deleteFeature(VectorFeature *feature)
{
    if (!feature) return false;
    if (auto ogrFeat = feature->getOGRFeature())
        return deleteFeature(ogrFeat->GetFID());
    return false;
}

void vrsa::vector::VectorLayer::setVisible(bool visible)
{
    for (const auto& feat : mFeatures)
    {
        if (feat)
            feat->setVisible(visible);
    }
    emit visibilityChanged(visible);
}

void vrsa::vector::VectorLayer::setZValue(int zValue) noexcept
{
    mZValue = zValue;
    emit ZValueChanged(mZValue);
}
vrsa::common::GeometryType vrsa::vector::VectorLayer::getGeomType() const
{
    return mOGRLayer ? gdalwrapper::FromOGR(mOGRLayer->GetGeomType()) : common::GeometryType::Unknown;
}

OGRwkbGeometryType vrsa::vector::VectorLayer::getOGRGeomType() const
{
    return mOGRLayer ? mOGRLayer->GetGeomType() : OGRwkbGeometryType::wkbUnknown;
}







std::vector<std::string> vrsa::vector::VectorLayer::getFieldNames() const
{
    if (!mFeatures.empty())
        return mFeatures[0]->getFieldNames();
    std::vector<std::string> names;
    if (mOGRLayer)
    {
        OGRFeatureDefn* poDefn = mOGRLayer->GetLayerDefn();
        for (int i = 0; i < poDefn->GetFieldCount(); ++i)
            names.push_back(poDefn->GetFieldDefn(i)->GetNameRef());
    }
    return names;
}

vrsa::common::FieldType vrsa::vector::VectorLayer::getFieldType(const std::string& fieldName) const
{
    if (!mFieldTypesCacheValid)
        buildFieldTypesCache();

    auto it = mFieldTypesCache.find(fieldName);
    if (it != mFieldTypesCache.end())
        return it->second;

    return vrsa::common::FieldType::String; // по умолчанию
}

bool vrsa::vector::VectorLayer::hasField(const std::string& fieldName) const
{
    auto names = getFieldNames();
    return std::find(names.begin(), names.end(), fieldName) != names.end();
}

int vrsa::vector::VectorLayer::getFieldCount() const noexcept
{
    return static_cast<int>(getFieldNames().size());
}


void vrsa::vector::VectorLayer::buildFieldTypesCache() const
{
    mFieldTypesCache.clear();

    //собираем типы из всех фич
    for (const auto& feature : mFeatures)
    {
        if (!feature) continue;
        auto fieldNames = feature->getFieldNames();
        for (const auto& fieldName : fieldNames)
        {
            auto fieldType = feature->getFieldType(fieldName);
            mFieldTypesCache[fieldName] = fieldType;
        }
    }

    mFieldTypesCacheValid = true;
}

void vrsa::vector::VectorLayer::setStyle(std::unique_ptr<graphics::VectorFeatureStyle> style, common::GeometryType geomType)
{
    mStyles.emplace(geomType, std::move(style));
}

vrsa::graphics::VectorFeatureStyle *vrsa::vector::VectorLayer::getStyle(common::GeometryType geomType) const noexcept
{
    auto it = mStyles.find(geomType);
    if (it != mStyles.end()) {
        return it->second.get();
    }
    return nullptr;
};



std::vector<vrsa::gdalwrapper::OgrFeaturePtr> vrsa::vector::VectorLayer::getClonedFilteredOGRFeatures(OGRGeometry *filter)
{
    if (!filter)
        return {};

    std::vector<vrsa::gdalwrapper::OgrFeaturePtr> result;
    //применяем фильтр к OGR слою
    mOGRLayer->SetSpatialFilter(filter);
    getClonedOGRFeatures();
    mOGRLayer->SetSpatialFilter(nullptr);
    //mOGRLayer->SetAttributeFilter(nullptr);
    return result;
}

std::vector<std::unique_ptr<vrsa::vector::VectorFeature> > vrsa::vector::VectorLayer::getClonedFilteredFeatures(OGRGeometry *filter)
{
    if (!filter)
        return {};
    std::vector<std::unique_ptr<VectorFeature>> result;

    mOGRLayer->SetSpatialFilter(filter);
    result = getClonedFeatures();
    mOGRLayer->SetSpatialFilter(nullptr);
    return result;
}

std::vector<std::unique_ptr<vrsa::vector::VectorFeature> > vrsa::vector::VectorLayer::getClonedFilteredFeatures(const geometry::Geometry &filter)
{
    return getClonedFilteredFeatures(ogr_utils::OGRConverter::toOGR(filter));
}

std::vector<std::unique_ptr<vrsa::vector::VectorFeature>> vrsa::vector::VectorLayer::
                        getClonedAttributeFilteredFeatures(const std::string &attributeFilter)
{
    if (!mOGRLayer)
        return {};
    if (attributeFilter.empty())
        return {};
    //устанавливаем атрибутный фильтр на OGR слой
    if (mOGRLayer->SetAttributeFilter(attributeFilter.c_str()) != OGRERR_NONE)
        VRSA_LOG_GDAL_ERROR("VectorLayer", "Invalid attribute filter: " + attributeFilter);

    auto result = getClonedFeatures();
    mOGRLayer->SetAttributeFilter(nullptr);
    return result;
}

std::vector<vrsa::gdalwrapper::OgrFeaturePtr> vrsa::vector::VectorLayer::getClonedAttributeFilteredOGRFeatures(const std::string &attributeFilter)
{
    if (!mOGRLayer)
        return {};
    if (attributeFilter.empty())
        return {};
    //устанавливаем атрибутный фильтр на OGR слой
    if (mOGRLayer->SetAttributeFilter(attributeFilter.c_str()) != OGRERR_NONE)
        VRSA_LOG_GDAL_ERROR("VectorLayer", "Invalid attribute filter: " + attributeFilter);

    auto result = getClonedOGRFeatures();
    mOGRLayer->SetAttributeFilter(nullptr);
    return result;
}

std::vector<vrsa::vector::VectorFeature*> vrsa::vector::VectorLayer::getOriginalAttributeFilteredFeatures(const std::string &attributeFilter)
{
    if (!mOGRLayer)
        return {};
    if (attributeFilter.empty())
        return {};
    //устанавливаем атрибутный фильтр на OGR слой
    if (mOGRLayer->SetAttributeFilter(attributeFilter.c_str()) != OGRERR_NONE)
        VRSA_LOG_GDAL_ERROR("VectorLayer", "Invalid attribute filter: " + attributeFilter);

    auto result = getOriginalFeatures();
    mOGRLayer->SetAttributeFilter(nullptr);
    return result;
}

std::vector<std::unique_ptr<vrsa::vector::VectorFeature> > vrsa::vector::VectorLayer::getClonedFeatures(bool reset)
{
    if (!mOGRLayer)
        return{};
    std::vector<std::unique_ptr<VectorFeature>> result;
    if (!hasFilters()) //если нет пространственного или атрибутивного фильтра - просто отдаем сырые указатели на VectorFeature
    {
        result.reserve(mFeatures.size());
        for (const auto& feat : mFeatures)
        {
            if (feat)
                result.push_back(feat.get()->clone());
            else
                VRSA_DEBUG("VectorLayer", "Nullptr feature in mFeatures!");
        }
        return result;
    }
    result.reserve(mOGRLayer->GetFeatureCount()); //возможно учитывает пространственный фильтр поэтому вызываем через mOGRLayer
    // Проходим по всем фичам
    if (reset)
        mOGRLayer->ResetReading();
    //т.к. GetNextFeature() возвращает КОПИЮ, оборачиваем в умный указатель, чтобы не вызывать вручную DestroyFeature()
    gdalwrapper::OgrFeaturePtr ogrFeat;
    while ((ogrFeat = gdalwrapper::OgrFeaturePtr(mOGRLayer->GetNextFeature())) != nullptr)
    {
        GIntBig fid = ogrFeat->GetFID(); //fid фичи под фильтром
        //находим совпадающие с пространственным фильтром VectorFeature
        auto it = std::find_if(mFeatures.begin(), mFeatures.end(),
                               [fid](const auto& feature) {
            return feature->getOGRFeature() && feature->getOGRFeature()->GetFID() == fid;
        });

        if (it != mFeatures.end())
            result.push_back((*it)->clone());
    }

    //mOGRLayer->SetSpatialFilter(nullptr);
    return result;
}

std::vector<vrsa::gdalwrapper::OgrFeaturePtr> vrsa::vector::VectorLayer::getClonedOGRFeatures(bool reset)
{
    if (!mOGRLayer)
        return{};
    std::vector<vrsa::gdalwrapper::OgrFeaturePtr> result;
    if (!hasFilters()) //если нет пространственного или атрибутивного фильтра - просто отдаем сырые указатели на VectorFeature
    {
        result.reserve(mFeatures.size());
        for (const auto& feat : mFeatures)
        {
            if (feat)
            {
                if (auto ogrFeat = feat->getOGRFeature())
                    result.emplace_back(gdalwrapper::OgrFeaturePtr(ogrFeat->Clone()));
            }
            else
                VRSA_DEBUG("VectorLayer", "Nullptr OGRfeature in mFeatures!");
        }
        return result;
    }
    result.reserve(mOGRLayer->GetFeatureCount()); //возможно учитывает пространственный фильтр поэтому вызываем через mOGRLayer
    if (reset)
        mOGRLayer->ResetReading();
    //т.к. GetNextFeature() возвращает КОПИЮ, оборачиваем в умный указатель и добавляем
    gdalwrapper::OgrFeaturePtr ogrFeat;
    while ((ogrFeat = gdalwrapper::OgrFeaturePtr(mOGRLayer->GetNextFeature())) != nullptr)
        result.push_back(std::move(ogrFeat));
    return result;
}

std::vector<vrsa::vector::VectorFeature*> vrsa::vector::VectorLayer::getOriginalFeatures(bool reset)
{
    if (!mOGRLayer)
        return{};
    std::vector<VectorFeature*> result;
    if (!hasFilters()) //если нет пространственного или атрибутивного фильтра - просто отдаем сырые указатели на VectorFeature
    {
        result.reserve(mFeatures.size());
        for (const auto& feat : mFeatures)
            if (feat)
                result.push_back(feat.get());
            else
                VRSA_DEBUG("VectorLayer", "Nullptr feature in mFeatures!");
        return result;
    }
    // если есть фильтр проходим по фичам через gdal api
    result.reserve(mOGRLayer->GetFeatureCount());
    if (reset)
        mOGRLayer->ResetReading();
    //т.к. GetNextFeature() возвращает КОПИЮ, оборачиваем в умный указатель, чтобы не вызывать вручную DestroyFeature()
    gdalwrapper::OgrFeaturePtr ogrFeat;
    while ((ogrFeat = gdalwrapper::OgrFeaturePtr(mOGRLayer->GetNextFeature())) != nullptr)
    {
        GIntBig fid = ogrFeat->GetFID(); //fid фичи под фильтром
        //находим совпадающие с пространственным фильтром VectorFeature
        auto it = std::find_if(mFeatures.begin(), mFeatures.end(),
                               [fid](const auto& feature) {
            return feature->getOGRFeature() && feature->getOGRFeature()->GetFID() == fid;
        });

        if (it != mFeatures.end())
            result.push_back(it->get());
    }
    return result;

}

bool vrsa::vector::VectorLayer::hasSpatialFilter() const
{
    if (!mOGRLayer)
        return false;
    return mOGRLayer->GetSpatialFilter() != nullptr;
}

bool vrsa::vector::VectorLayer::hasAttributeFilter() const
{
    if (!mOGRLayer)
        return false;
    return mOGRLayer->GetAttrQueryString() != nullptr;
}

bool vrsa::vector::VectorLayer::hasFilters() const
{
    return hasSpatialFilter() || hasAttributeFilter();
}

bool vrsa::vector::VectorLayer::setSpatialFilter(OGRGeometry *filter)
{
    if (!mOGRLayer || !filter)
        return false;
    mOGRLayer->SetSpatialFilter(filter);
    return true;
}

bool vrsa::vector::VectorLayer::setSpatialFilter(geometry::Geometry filter)
{
    auto ogrGeom = ogr_utils::OGRConverter::toOGR(filter);
    if (!mOGRLayer || !ogrGeom)
        return false;
    mOGRLayer->SetSpatialFilter(ogrGeom);
    return true;

}

bool vrsa::vector::VectorLayer::setAttributeFilter(const std::string &filter)
{
    if(!mOGRLayer && filter.empty())
        return false;
    if (mOGRLayer->SetAttributeFilter(filter.c_str()) != OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("VectorLayer", "Invalid attribute filter: " + filter);
        return false;
    }
    return true;
}

OGRGeometry *vrsa::vector::VectorLayer::getSpatialOGRFilter()
{
    if (hasSpatialFilter())
        return mOGRLayer->GetSpatialFilter();
    return nullptr;
}

vrsa::geometry::Geometry vrsa::vector::VectorLayer::getSpatialFilter()
{
    if (hasSpatialFilter())
        return ogr_utils::OGRConverter::fromOGR(mOGRLayer->GetSpatialFilter());
    geometry::Geometry unknown;
    unknown.type = common::GeometryType::Unknown;
    return unknown;
}

std::string vrsa::vector::VectorLayer::getAttributeFilter(const std::string &filter)
{
    if (hasAttributeFilter())
        return mOGRLayer->GetAttrQueryString();
    return {};
}



bool vrsa::vector::VectorLayer::hasMultiGeometry() const
{
    for (const auto& feature : mFeatures) {
        if (feature && feature->isMultiGeometry()) {
            return true;
        }
    }
    return false;
}

std::unique_ptr<vrsa::vector::VectorLayer> vrsa::vector::VectorLayer::createExplodedLayer(const std::string& newLayerName) const
{
    std::string layerName = newLayerName.empty() ?
                std::string(getName()) + "_exploded" : newLayerName;

    auto newLayer = std::make_unique<VectorLayer>(nullptr);
    // Здесь нужно создать новый слой без OGRLayer или с новым OGRLayer

    // Копируем стили
    for (const auto& [geomType, style] : mStyles) {
        // Создаем копию стиля (нужен метод clone в VectorFeatureStyle)
        // newLayer->setStyle(std::unique_ptr<graphics::VectorFeatureStyle>(style->clone()), geomType);
    }

    // Разбиваем все фичи
    for (const auto& feature : mFeatures) {
        if (feature && feature->isMultiGeometry()) {
            auto exploded = feature->explodeToSimpleFeatures();
            for (auto& newFeature : exploded) {
                newLayer->addFeature(std::move(newFeature));
            }
        } else if (feature) {
            newLayer->addFeature(feature->clone());
        }
    }

    return newLayer;
}

GDALDataset* vrsa::vector::VectorLayer::exportToGDALDataset(const std::string& format,
                                                            const std::string& outputPath,
                                                            bool explodeMultiGeometries) const
{
    GDALAllRegister();

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
    if (!poDriver) {
        VRSA_DEBUG("ERROR", "Driver not found: " + format);
        return nullptr;
    }

    GDALDataset* poDS;
    if (outputPath.empty()) {
        poDS = poDriver->Create("/vsimem/exported_layer", 0, 0, 0, GDT_Unknown, NULL);
    } else {
        poDS = poDriver->Create(outputPath.c_str(), 0, 0, 0, GDT_Unknown, NULL);
    }

    if (!poDS) {
        VRSA_DEBUG("ERROR", "Failed to create dataset");
        return nullptr;
    }

    // Определяем тип геометрии
    OGRwkbGeometryType targetGeomType;
    if (explodeMultiGeometries) {
        // Если разбиваем, то используем простые типы
        targetGeomType = gdalwrapper::GeometryTypeConverter::getSimpleGeometryType(gdalwrapper::GeometryTypeConverter::ToOGR(getGeomType()));
    } else {
        targetGeomType = gdalwrapper::GeometryTypeConverter::ToOGR(getGeomType());
    }

    // Создаем слой
    OGRLayer* poLayer = poDS->CreateLayer(
                getName(),
                nullptr,
                targetGeomType,
                NULL
                );

    if (!poLayer) {
        VRSA_DEBUG("ERROR", "Failed to create layer");
        GDALClose(poDS);
        return nullptr;
    }

    // Создаем поля
    auto fieldNames = getFieldNames();
    for (const auto& fieldName : fieldNames) {
        auto fieldType = getFieldType(fieldName);
        OGRFieldDefn ogrField(fieldName.c_str(), gdalwrapper::GeometryTypeConverter::convertToOGRFieldType(fieldType));
        poLayer->CreateField(&ogrField);
    }

    // Добавляем служебные поля
    OGRFieldDefn originalFidField("_original_fid", OFTInteger);
    poLayer->CreateField(&originalFidField);

    OGRFieldDefn partIndexField("_part_index", OFTInteger);
    poLayer->CreateField(&partIndexField);

    // Заполняем данными
    int featureCount = 0;
    int originalFid = 0;

    for (const auto& feature : mFeatures) {
        if (!feature) continue;

        if (explodeMultiGeometries && feature->isMultiGeometry()) {
            auto exploded = feature->explodeToSimpleFeatures();
            for (size_t partIdx = 0; partIdx < exploded.size(); partIdx++) {
                OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

                // Устанавливаем геометрию
                OGRGeometry* ogrGeom = exploded[partIdx]->getOGRGeometry();
                if (ogrGeom) {
                    poFeature->SetGeometry(ogrGeom);
                }

                // Копируем атрибуты
                auto attributes = feature->getAttributesAsString();
                for (const auto& [fieldName, fieldValue] : attributes) {
                    int fieldIndex = poFeature->GetDefnRef()->GetFieldIndex(fieldName.c_str());
                    if (fieldIndex >= 0) {
                        poFeature->SetField(fieldIndex, fieldValue.c_str());
                    }
                }

                poFeature->SetField("_original_fid", originalFid);
                poFeature->SetField("_part_index", (int)partIdx);

                if (poLayer->CreateFeature(poFeature) == OGRERR_NONE) {
                    featureCount++;
                }

                OGRFeature::DestroyFeature(poFeature);
            }
        }
        else
        {
            OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

            OGRGeometry* ogrGeom = feature->getOGRGeometry();
            if (ogrGeom) {
                poFeature->SetGeometry(ogrGeom);
            }

            auto attributes = feature->getAttributesAsString();
            for (const auto& [fieldName, fieldValue] : attributes) {
                int fieldIndex = poFeature->GetDefnRef()->GetFieldIndex(fieldName.c_str());
                if (fieldIndex >= 0) {
                    poFeature->SetField(fieldIndex, fieldValue.c_str());
                }
            }

            poFeature->SetField("_original_fid", originalFid);
            poFeature->SetField("_part_index", 0);

            if (poLayer->CreateFeature(poFeature) == OGRERR_NONE) {
                featureCount++;
            }

            OGRFeature::DestroyFeature(poFeature);
        }

        originalFid++;
    }

    VRSA_DEBUG("INFO", "Exported " + std::to_string(featureCount) +
               " features to dataset");

    return poDS;
}


//OGRwkbGeometryType vrsa::vector::VectorLayer::convertToOGRGeometryType(vrsa::common::GeometryType type) const
//{
//    switch(type) {
//        case vrsa::common::GeometryType::Point: return wkbPoint;
//        case vrsa::common::GeometryType::LineString: return wkbLineString;
//        case vrsa::common::GeometryType::Polygon: return wkbPolygon;
//        case vrsa::common::GeometryType::MultiPoint: return wkbMultiPoint;
//        case vrsa::common::GeometryType::MultiLineString: return wkbMultiLineString;
//        case vrsa::common::GeometryType::MultiPolygon: return wkbMultiPolygon;
//        default: return wkbUnknown;
//    }
//}

//OGRFieldType vrsa::vector::VectorLayer::convertToOGRFieldType(vrsa::common::FieldType type) const
//{
//    switch(type) {
//        case vrsa::common::FieldType::Integer: return OFTInteger;
//        case vrsa::common::FieldType::Integer64: return OFTInteger64;
//        case vrsa::common::FieldType::Real: return OFTReal;
//        case vrsa::common::FieldType::String: return OFTString;
//        case vrsa::common::FieldType::Date: return OFTDate;
//        case vrsa::common::FieldType::DateTime: return OFTDateTime;
//        case vrsa::common::FieldType::Boolean: return OFTInteger; // bool как integer
//        default: return OFTString;
//    }
//}

//OGRwkbGeometryType vrsa::vector::VectorLayer::getSimpleGeometryType(OGRwkbGeometryType type) const
//{
//    OGRwkbGeometryType flat = wkbFlatten(type);
//    switch(flat) {
//        case wkbMultiPoint: return wkbPoint;
//        case wkbMultiLineString: return wkbLineString;
//        case wkbMultiPolygon: return wkbPolygon;
//        case wkbGeometryCollection: return wkbUnknown;
//        default: return flat;
//    }
//}

//std::vector<std::string> vrsa::vector::VectorLayer::getAttributesNames() const
//{
//    OGRFeatureDefn* defn = mOGRLayer->GetLayerDefn();
//    std::vector<std::string> names;
//    names.reserve(defn->GetFieldCount());
//    for (int i = 0; i < defn->GetFieldCount(); ++i)
//    {
//        names.push_back(defn->GetFieldDefn(i)->GetNameRef());
//    }
//    return names;

//}
