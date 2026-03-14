#include "vectorfeature.h"
#include "vector/vectorlayer.h"
#include "gdal/ogrconverter.h"
#include "gdal/geometrytypeconverter.h"
#include "geometry/geometry.h"
#include "graphics/vectorfeaturestyle.h"
#include "gdal/typeconversions.h"
vrsa::vector::VectorFeature::VectorFeature(vrsa::gdalwrapper::OgrFeaturePtr rawFeature, OGRLayer* layer)
    : mFeature{std::move(rawFeature)},
      mParentLayer{layer},
      mZValue{-1},
      mStyle{nullptr}
{
    if (mFeature)
    {
        //это очень замедляет загрузку больших слоев с большим количеством атрибутов, возможно не стоит так делать ...
        constructFromOGRfeature(mFeature.get());  //cинхронизируем атрибуты
        setVisible(true);
    }
}

bool vrsa::vector::VectorFeature::setGeometry(gdalwrapper::OgrGeometryPtr ptr)
{
    if (!mFeature)
        return false;
    if (!ptr)
    {
        VRSA_ERROR("VectorFeature", "Error while setting geometry: nullptr OgrGeometryPtr");
        return false;
    }

    //клонируем для передачи во владение OGRFeature
    OGRGeometry* clone = ptr->clone();
    if (!clone)
        return false;

    // OGRFeature сам удалит старую геометрию
    OGRErr err = mFeature->SetGeometry(clone);
    if (err != OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("VectorFeature", "Can't set new geometry to OGRFeature");
        OGRGeometryFactory::destroyGeometry(clone);
        return false;
    }

    emit geometryChanged();
    emit featureChanged();
    return true;
}

bool vrsa::vector::VectorFeature::setGeometry(OGRGeometry *ogrGeom)
{
    if (!mFeature)
        return false;
    if (!ogrGeom)
    {
        VRSA_ERROR("VectorFeature", "Error while setting geometry: nullptr OGRGeometry pointer");
        return false;
    }

    // cоздаем копию
    OGRGeometry* clone = ogrGeom->clone();
    if (!clone) return false;

    // передаем владение указателем фиче
    OGRErr err = mFeature->SetGeometry(clone);
    if (err != OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("VectorFeature", "Can't set new geometry to OGRFeature");
        OGRGeometryFactory::destroyGeometry(clone);
        return false;
    }

    emit geometryChanged();
    emit featureChanged();
    return true;
}

bool vrsa::vector::VectorFeature::setGeometry(const geometry::Geometry &geometry)
{

//    qDebug()<< "VECTOR FEATURE OLD GEOM - " << mFeature->GetGeometryRef();
//    char* wkt = nullptr;
//    mFeature->GetGeometryRef()->exportToWkt(&wkt);
//    qDebug() << "OLD GEOM WKT:" << wkt;
//    CPLFree(wkt);
    auto ogrGeomUPtr = ogr_utils::OGRConverter::toOGR_uniquePTR(geometry);
    if (!ogrGeomUPtr)
    {
        VRSA_DEBUG("VectorFeature", "Can't convert internal geometry to OGRGeometry.");
        return false;
    }
    auto setGeomError = mFeature->SetGeometry(ogrGeomUPtr.get());
    if (setGeomError != OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("VectorFeature", "Can't set new geometry to OGRFeature");
        return false;
    }
    else if (!mParentLayer)
    {
        VRSA_DEBUG("VectorFeature", "Geometry was successfully setted to the feature, but parent layer wasn't found");
        return true;
    }

    auto setFeatureError = mParentLayer->SetFeature(mFeature.get());
    if (setFeatureError != OGRERR_NONE)
    {
        const char* errorMsg = CPLGetLastErrorMsg();
        qDebug()<<"vectorfeature " << errorMsg;
        VRSA_LOG_GDAL_ERROR("VectorFeature", "Geometry was successfully set to the feature, but failed to update in OGRLayer."
                            "The feature may not have been added to the layer yet. If you digitizing, ignore this error. ");

        return true;
    }
    VRSA_DEBUG("VectorFeature", "Geometry was successfully set to the feature and updated in OGRLayer.");
//    qDebug()<< "VECTOR FEATURE NEW GEOM - " << mFeature->GetGeometryRef();
//    char* WKTNEW = nullptr;
//    mFeature->GetGeometryRef()->exportToWkt(&WKTNEW);
//    qDebug() << "NEW GEOM WKT:" << WKTNEW;
//    CPLFree(WKTNEW);
    emit geometryChanged();
    emit featureChanged();
    return true;
}


void vrsa::vector::VectorFeature::setVisible(bool visible)
{
    mIsVisible = visible;
    emit visibilityChanged(visible);
}

void vrsa::vector::VectorFeature::setSelected(bool selected)
{
    mIsSelected = selected;
    emit selectionChanged(mIsSelected);
}

void vrsa::vector::VectorFeature::setStyle(graphics::VectorFeatureStyle *newStyle)
{
    mStyle = newStyle;
    emit styleChanged(mStyle);
}

void vrsa::vector::VectorFeature::setZValue(int zVal)
{
    mZValue = zVal;
    emit ZValueChanged(mZValue);
}

vrsa::common::GeometryType vrsa::vector::VectorFeature::getType() const
{
    if (!mFeature)
        return vrsa::common::GeometryType::Unknown;
    auto featGeom = mFeature->GetGeometryRef();
    if (!featGeom)
        return vrsa::common::GeometryType::Unknown;
    return vrsa::gdalwrapper::GeometryTypeConverter::FromOGR(featGeom->getGeometryType());
}

vrsa::common::GeometryType vrsa::vector::VectorFeature::getGeometryType() const
{
    if (!mFeature)
        return common::GeometryType::Unknown;
    auto featGeom = mFeature->GetGeometryRef();
    return featGeom ? gdalwrapper::FromOGR(featGeom->getGeometryType()) : common::GeometryType::Unknown;
}

OGRwkbGeometryType vrsa::vector::VectorFeature::getOGRGeometryType() const
{
    if (!mFeature)
        return OGRwkbGeometryType::wkbUnknown;
    auto featGeom = mFeature->GetGeometryRef();
    return featGeom ? featGeom->getGeometryType() : OGRwkbGeometryType::wkbUnknown;

}

OGRGeometry *vrsa::vector::VectorFeature::getOGRGeometry() const
{
    //возвращается оригинальная геометрия
    return mFeature ? mFeature->GetGeometryRef(): nullptr;
}

vrsa::gdalwrapper::OgrGeometryPtr vrsa::vector::VectorFeature::cloneOGRGeometry() const
{
    if (!mFeature) return nullptr;
    OGRGeometry* geom = mFeature->GetGeometryRef(); //для клонирования
    return geom ? vrsa::gdalwrapper::OgrGeometryPtr(geom->clone()) : nullptr;
}

vrsa::geometry::Geometry vrsa::vector::VectorFeature::getInternalGeometry() const
{
    return mFeature ? ogr_utils::OGRConverter::fromOGR(mFeature->GetGeometryRef()) : geometry::Geometry();
}

vrsa::vector::VectorFeature::AttributeValue vrsa::vector::VectorFeature::getAttribute(const std::string &name) const
{
    auto it = mAttributes.find(name);
    if (it != mAttributes.end())
        return it->second;
    return nullptr;
}



std::unordered_map<std::string, std::string> vrsa::vector::VectorFeature::getAttributesAsString() const
{
    std::unordered_map<std::string, std::string> attributes;
    attributes.reserve(mAttributes.size());
    for (const auto& [name, value] : mAttributes)
        attributes.emplace(name, getAttributeAsString(name));
    return attributes;
}

std::unordered_map<QString, QString> vrsa::vector::VectorFeature::getAttributesAsQString() const
{
    std::unordered_map<QString, QString> attributes;
    for (const auto& attr: mAttributes)
    {
//        qDebug()<<QString::fromStdString(attr.first);
//        qDebug()<<QString::fromStdString(getAttributeAsString(attr.first));
        attributes.insert({QString::fromStdString(attr.first), getAttributeAsQString(attr.first)});
    }
    return attributes;
}

std::string vrsa::vector::VectorFeature::getAttributeAsString(const std::string &name) const
{
    auto it = mAttributes.find(name);
    if (it != mAttributes.end())
    {
        auto value = it->second;
        return std::visit([](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return ("NULL"); // или QVariant()
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, double>) {
                return std::to_string(arg);
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "True" : "False";
            }
            else {
                return "NULL_";
            }
        }, value);
    }
    return "NULL__";
}

QString vrsa::vector::VectorFeature::getAttributeAsQString(const std::string &name) const
{
    return QString::fromStdString(getAttributeAsString(name));
}



std::unique_ptr<vrsa::vector::VectorFeature> vrsa::vector::VectorFeature::createFeature(VectorLayer *parentLayer)
{
    if (!parentLayer)
    {
        VRSA_ERROR("VectorFeature", "Can't create feature: passed VectorLayer nullptr");
        return nullptr;
    }
    OGRLayer* ogrL = parentLayer->getOGRLayer();
    if (!ogrL)
    {
        VRSA_ERROR("VectorFeature", "Can't create feature: passed VectorLayer has nullptr OGRLayer");
        return nullptr;
    }
    OGRFeature* feature = OGRFeature::CreateFeature(ogrL->GetLayerDefn());
    if (!feature)
    {
        VRSA_ERROR("VectorFeature", "Can't create feature: passed VectorLayer has incorrect layer definition");
        return nullptr;
    }
    auto vrsaFeat = std::make_unique<VectorFeature>(vrsa::gdalwrapper::OgrFeaturePtr(feature), ogrL);
    vrsaFeat->setStyle(parentLayer->getStyle());
    vrsaFeat->setZValue(parentLayer->getZValue());
    VRSA_DEBUG("VectorFeature", "Feature successfully created");
    return vrsaFeat;
}





std::vector<std::string> vrsa::vector::VectorFeature::getFieldNames() const
{
    std::vector<std::string> names;
    for (const auto& [name, _] : mAttributes)
        names.push_back(name);
    return names;
}

vrsa::common::FieldType vrsa::vector::VectorFeature::getFieldType(const std::string& name) const
{
    auto attrIt = mAttributes.find(name);
    if (attrIt != mAttributes.end())
    {
        if (std::holds_alternative<int64_t>(attrIt->second))
            return common::FieldType::Integer;
        if (std::holds_alternative<double>(attrIt->second))
            return common::FieldType::Real;
        if (std::holds_alternative<bool>(attrIt->second))
            return common::FieldType::Boolean;
        if (std::holds_alternative<std::string>(attrIt->second))
            return common::FieldType::String;
    }
    return common::FieldType::Unknown;
}

bool vrsa::vector::VectorFeature::hasField(const std::string& name) const
{
    return mAttributes.find(name) != mAttributes.end();
}

int vrsa::vector::VectorFeature::getFieldCount() const
{
    return static_cast<int>(mAttributes.size());
}

bool vrsa::vector::VectorFeature::isMultiGeometry() const
{
    if (!mFeature) return false;
    auto geom = mFeature->GetGeometryRef();
    if (!geom) return false;

    OGRwkbGeometryType type = wkbFlatten(geom->getGeometryType());
    return type == wkbMultiPoint ||
            type == wkbMultiLineString ||
            type == wkbMultiPolygon ||
            type == wkbGeometryCollection;
}

std::vector<std::unique_ptr<vrsa::vector::VectorFeature>> vrsa::vector::VectorFeature::explodeToSimpleFeatures() const
{
    std::vector<std::unique_ptr<VectorFeature>> result;
    if (!mFeature) return result;
    auto geom = mFeature->GetGeometryRef();
    if (!geom) return result;

    if (!isMultiGeometry()) {
        //если уже простая геометрия, просто возвращаем копию
        result.push_back(clone());
        return result;
    }

    auto* collection = dynamic_cast<OGRGeometryCollection*>(geom);
    if (!collection) return result;

    for (int i = 0; i < collection->getNumGeometries(); ++i) {
        OGRGeometry* subGeom = collection->getGeometryRef(i);
        if (!subGeom) continue;


        auto newFeature = std::make_unique<VectorFeature>(nullptr, mParentLayer);

        //копируем геометрию и атрибуты
        bool flag = newFeature->setGeometry(vrsa::gdalwrapper::OgrGeometryPtr(subGeom->clone()));
        if (flag)
            VRSA_DEBUG("VectorFeature", "Can't set geometry to new simple feature");

        for (const auto& [name, value] : mAttributes) {
            newFeature->setAttribute(name, value);
        }

        //служебные атрибуты
        newFeature->setAttribute("_original_index", i);
        newFeature->setAttribute("_original_fid", (int)this->getOGRFeature()->GetFID());

        result.push_back(std::move(newFeature));
    }

    return result;
}

std::unique_ptr<vrsa::vector::VectorFeature> vrsa::vector::VectorFeature::clone() const
{
    if (!mFeature) return nullptr;
    auto geom = mFeature->GetGeometryRef();
    OGRFeature* clonedOGR = mFeature->Clone();
    if (!clonedOGR) return nullptr;
    auto newFeature = std::make_unique<VectorFeature>(gdalwrapper::OgrFeaturePtr(clonedOGR), mParentLayer);
    if (!newFeature) return nullptr;
    if (geom)
    {
        bool flag = newFeature->setGeometry(vrsa::gdalwrapper::OgrGeometryPtr(geom->clone()));
        if (!flag)
            VRSA_DEBUG("VectorFeature", "Can't clone() geometry to new feature");
    }

    for (const auto& [name, value] : mAttributes)
        newFeature->setAttribute(name, value);

    //newFeature->setName(mName);
    newFeature->setVisible(mIsVisible);

    return newFeature;
}


bool vrsa::vector::VectorFeature::setAttribute(const std::string& name, const QVariant& value)
{       
    if (value.type() == QVariant::Int)
        return setAttribute(name, value.toInt());
    else if (value.type() == QVariant::LongLong)
            return setAttribute(name, static_cast<int64_t>(value.toLongLong()));
    else if (value.type() == QVariant::Double)
        return setAttribute(name, value.toDouble());
    else if (value.type() == QVariant::Bool)
        return setAttribute(name, value.toBool());
    else
        return setAttribute(name, value.toString().toStdString());
    //syncToOGRFeature(name, value);
}

bool vrsa::vector::VectorFeature::setAttribute(const std::string& name, int value)
{
    return setAttribute(name, static_cast<int64_t>(value));
}

bool vrsa::vector::VectorFeature::setAttribute(const std::string &name, int64_t value)
{
    if (syncToOGRFeature(name, value))
    {
        mAttributes[name] = value;
        return true;
    }
    return false;
}

bool vrsa::vector::VectorFeature::setAttribute(const std::string& name, double value)
{
    if (syncToOGRFeature(name, value))
    {
        mAttributes[name] = value;
        return true;
    }
    return false;
}

bool vrsa::vector::VectorFeature::setAttribute(const std::string& name, const std::string& value)
{
    if (syncToOGRFeature(name, value))
    {
        mAttributes[name] = value;
        return true;
    }
    return false;
}

bool vrsa::vector::VectorFeature::setAttribute(const std::string& name, bool value)
{
    if (syncToOGRFeature(name, value))
    {
        mAttributes[name] = value;
        return true;
    }
    return false;

}

bool vrsa::vector::VectorFeature::setAttribute(const std::string& name, const char* value)
{
    return setAttribute(name, std::string(value));
}

bool vrsa::vector::VectorFeature::setAttribute(const std::string &name, const AttributeValue &value)
{
    if (std::holds_alternative<int64_t>(value))
        return setAttribute(name, std::get<int64_t>(value));
    else if (std::holds_alternative<double>(value))
        return setAttribute(name, std::get<double>(value));
    else if (std::holds_alternative<bool>(value))
        return setAttribute(name, std::get<bool>(value));
    else if (std::holds_alternative<std::string>(value))
        return setAttribute(name, std::get<std::string>(value));
    else if (std::holds_alternative<std::nullptr_t>(value))
    {
        if (mFeature && mParentLayer)
        {
            int fieldIndex = mFeature->GetFieldIndex(name.c_str());
            if (fieldIndex >= 0)
            {
                mFeature->SetFieldNull(fieldIndex);
                if (mParentLayer->SetFeature(mFeature.get()) == OGRERR_NONE)
                {
                    mAttributes[name] = value;
                    return true;
                }
                else
                {
                    VRSA_ERROR("VectorFeature", "Can't update attribute in a feature. Fid: "
                               + std::to_string(mFeature->GetFID()));
                    return false;
                }

            }
            return false;
        }
        return false;
    }
    return false;

}



void vrsa::vector::VectorFeature::constructFromOGRfeature(OGRFeature* feature)
{
    if (!feature) return;

    OGRFeatureDefn* defn = feature->GetDefnRef();
    for (int i = 0; i < defn->GetFieldCount(); ++i)
    {
        if (!feature->IsFieldSet(i)) continue;

        OGRFieldDefn* fieldDef = defn->GetFieldDefn(i);
        std::string name = fieldDef->GetNameRef();

        switch(fieldDef->GetType())
        {
        case OFTInteger:
            mAttributes[name] = feature->GetFieldAsInteger(i);
            break;
        case OFTInteger64:
            mAttributes[name] = feature->GetFieldAsInteger64(i);
            break;
        case OFTReal:
            mAttributes[name] = feature->GetFieldAsDouble(i);
            break;
        case OFTString:
            mAttributes[name] = std::string(feature->GetFieldAsString(i));
            break;
        default:
            mAttributes[name] = std::string(feature->GetFieldAsString(i));
        }
    }
}


QVariant vrsa::vector::VectorFeature::getAttributeAsQVariant(const std::string& name) const
{
    auto it = mAttributes.find(name);
    if (it == mAttributes.end()) return QVariant();

    if (std::holds_alternative<int64_t>(it->second))
        return QVariant(static_cast<long long>(std::get<int64_t>(it->second)));
    if (std::holds_alternative<double>(it->second))
        return QVariant(std::get<double>(it->second));
    if (std::holds_alternative<bool>(it->second))
        return QVariant(std::get<bool>(it->second));
    if (std::holds_alternative<std::string>(it->second))
        return QVariant(QString::fromStdString(std::get<std::string>(it->second)));

    return QVariant();
}

std::vector<std::string> vrsa::vector::VectorFeature::getAttributeNames() const
{
    std::vector<std::string> names;
    for (const auto& [name, _] : mAttributes)
        names.push_back(name);
    return names;
}

