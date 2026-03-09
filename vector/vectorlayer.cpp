#include "vectorlayer.h"

vrsa::vector::VectorLayer::VectorLayer(OGRLayer *l)
    :mOGRLayer{l}
{

}

OGRLayer *vrsa::vector::VectorLayer::getOGRLayer()
{
    return mOGRLayer;
}

QRectF vrsa::vector::VectorLayer::getBoundingBox()
{
    OGREnvelope envelope;
    auto er=mOGRLayer->GetExtent(&envelope, true);
    if (er!=OGRERR_NONE)
        return QRectF();
    qreal minX = envelope.MinX;
    qreal minY = envelope.MinY;
    qreal maxX = envelope.MaxX;
    qreal maxY = envelope.MaxY;
    return QRectF(QPointF(minX, minY), QSizeF(maxX - minX, maxY - minY));
}

std::vector<vrsa::vector::VectorFeature *> vrsa::vector::VectorLayer::getFeatures()
{
    std::vector<VectorFeature*> ftrPtrs;
    ftrPtrs.reserve(mFeatures.size());
    for (const auto& layer : mFeatures)
    {
        ftrPtrs.push_back(layer.get());
    }
    return ftrPtrs;
}

std::vector<std::string> vrsa::vector::VectorLayer::getAttributesNames() const
{
    OGRFeatureDefn* defn = mOGRLayer->GetLayerDefn();
    std::vector<std::string> names;
    names.reserve(defn->GetFieldCount());
    for (int i = 0; i < defn->GetFieldCount(); ++i)
    {
        names.push_back(defn->GetFieldDefn(i)->GetNameRef());
    }
    return names;

}

vrsa::vector::VectorFeature* vrsa::vector::VectorLayer::getFeatureAt(size_t idx)
{
    return mFeatures.at(idx).get();
}

void vrsa::vector::VectorLayer::setVisible(bool visible)
{
    for (auto& feat : mFeatures)
    {
        feat->setVisible(visible);
    }

}

int vrsa::vector::VectorLayer::id()
{
    return 2;
}

void vrsa::vector::VectorLayer::setFeatures(featuresVec features)
{
    mFeatures = std::move(features);
}

vrsa::common::GeometryType vrsa::vector::VectorLayer::getGeomType() const
{
    return gdalwrapper::GeometryTypeConverter::FromOGR(mOGRLayer->GetGeomType());
}



bool vrsa::vector::VectorLayer::addFeature(std::unique_ptr<VectorFeature> feature)
{
    auto ogrF = feature->getOGRFeature();
    if (!ogrF)
    {
        VRSA_ERROR("VectorLayer", "NUFeature added!");
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
        VRSA_ERROR("VectorLayer", "Can't add vector feature to layer, OgrError:" + std::to_string(err));
        return false;
    }
}

bool vrsa::vector::VectorLayer::deleteFeature(GIntBig fid)
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
        if (ogrF)
            return ogrF->GetFID() == fid;
    });
    mFeatures.erase(it, mFeatures.end());
    mOGRLayer->SyncToDisk();
    return true;
}

bool vrsa::vector::VectorLayer::deleteFeature(VectorFeature *feature)
{
    if (!feature) return false;
    return deleteFeature(feature->getOGRFeature()->GetFID());
}



const char *vrsa::vector::VectorLayer::getName() const
{
    return mOGRLayer->GetName();
}


std::vector<std::string> vrsa::vector::VectorLayer::getFieldNames() const
{
    if (!mFeatures.empty()) {
        return mFeatures[0]->getFieldNames();
    }

    // Если нет фич, берем из OGR слоя
    std::vector<std::string> names;
    if (mOGRLayer) {
        OGRFeatureDefn* poDefn = mOGRLayer->GetLayerDefn();
        for (int i = 0; i < poDefn->GetFieldCount(); i++) {
            names.push_back(poDefn->GetFieldDefn(i)->GetNameRef());
        }
    }
    return names;
}

vrsa::common::FieldType vrsa::vector::VectorLayer::getFieldType(const std::string& fieldName) const
{
    if (!mFieldTypesCacheValid) {
        buildFieldTypesCache();
    }

    auto it = mFieldTypesCache.find(fieldName);
    if (it != mFieldTypesCache.end()) {
        return it->second;
    }

    return vrsa::common::FieldType::String; // по умолчанию
}

bool vrsa::vector::VectorLayer::hasField(const std::string& fieldName) const
{
    auto names = getFieldNames();
    return std::find(names.begin(), names.end(), fieldName) != names.end();
}

int vrsa::vector::VectorLayer::getFieldCount() const
{
    return static_cast<int>(getFieldNames().size());
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

// Вспомогательные методы
void vrsa::vector::VectorLayer::buildFieldTypesCache() const
{
    mFieldTypesCache.clear();

    // Собираем типы из всех фич
    for (const auto& feature : mFeatures) {
        if (!feature) continue;

        auto fieldNames = feature->getFieldNames();
        for (const auto& fieldName : fieldNames) {
            auto fieldType = feature->getFieldType(fieldName);
            mFieldTypesCache[fieldName] = fieldType;
        }
    }

    mFieldTypesCacheValid = true;
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
