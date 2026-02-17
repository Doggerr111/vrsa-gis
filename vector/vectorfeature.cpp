#include "vectorfeature.h"
#include "vector/vectorlayer.h"
vrsa::vector::VectorFeature::VectorFeature(vrsa::gdalwrapper::OgrFeaturePtr feature, OGRLayer* layer)
    : mFeature{std::move(feature)},
      mParentLayer{layer}
{

}

void vrsa::vector::VectorFeature::setGeometry(gdalwrapper::OgrGeometryPtr ptr)
{
    //mGeometry = std::move(ptr);
    //    emit geometryChanged();
    //    emit featureChanged();
}



void vrsa::vector::VectorFeature::setName(const std::string& name) {
    if (mName != name) {
        mName = name;
        //emit featureChanged();
    }
}


void vrsa::vector::VectorFeature::setAttribute(const std::string& name, const AttributeValue& value) {
    mAttributes[name] = value;
    //emit attributesChanged();
    //emit featureChanged();
}

void vrsa::vector::VectorFeature::setVisible(bool visible)
{
    mVisible = visible;
    emit visibilityChanged(visible);
}

vrsa::common::GeometryType vrsa::vector::VectorFeature::getType() const
{
    return vrsa::gdalwrapper::GeometryTypeConverter::FromOGR(mFeature->GetGeometryRef()->getGeometryType());
}

OGRGeometry *vrsa::vector::VectorFeature::getOGRGeometry() const
{
    return mFeature->GetGeometryRef();
}

vrsa::vector::VectorFeature::AttributeValue vrsa::vector::VectorFeature::getAttribute(const std::string &name) const
{

    auto it = mAttributes.find(name);
    if (it != mAttributes.end())
        return it->second;

    //    if (mFeature)
    //    {
    //        int fieldIndex = mFeature->GetFieldIndex(name.c_str());
    //        if (fieldIndex >= 0)
    //        {
    //            AttributeValue value = readAttributeFromOGR(fieldIndex);
    //            const_cast<VectorFeature*>(this)->mAttributes[name] = value;
    //            return value;
    //        }
    //    }

    return nullptr;

}

OGRFeature *vrsa::vector::VectorFeature::getOGRFeature() const
{
    return mFeature.get();
}

bool vrsa::vector::VectorFeature::updateAttribute(const std::string &name, const AttributeValue &value)
{
    if (!mFeature)
    {
        VRSA_ERROR("VectorFeature", "Cannot update attribute: OGRFeature is null");
        return false;
    }

    int fieldIndex = mFeature->GetFieldIndex(name.c_str());
    if (fieldIndex < 0)
    {
        VRSA_ERROR("VectorFeature", "Attribute " + name + "not found in OGRFeature schema");
        return false;
    }

    OGRFeatureDefn* defn = mFeature->GetDefnRef();
    if (!defn) return false;

    OGRFieldDefn* fieldDef = defn->GetFieldDefn(fieldIndex);
    OGRFieldType fieldType = fieldDef->GetType();

    try {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, std::nullptr_t>)
            {
                mFeature->SetFieldNull(fieldIndex);
                if (fieldType == OFTInteger || fieldType == OFTInteger64)
                {
                    mFeature->SetField(fieldIndex, 0);
                }
                else if (fieldType == OFTReal)
                {
                    mFeature->SetField(fieldIndex, 0.0);
                }
            }
            else if constexpr (std::is_same_v<T, int>)
            {

                if (fieldType == OFTInteger || fieldType == OFTInteger64)
                {
                    mFeature->SetField(fieldIndex, arg);
                }
                else if (fieldType == OFTReal)
                {
                    mFeature->SetField(fieldIndex, static_cast<double>(arg));
                }
                else if (fieldType == OFTString)
                {
                    mFeature->SetField(fieldIndex, std::to_string(arg).c_str());
                }
                else
                {
                    throw std::runtime_error("Type mismatch for integer value");
                }
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                if (fieldType == OFTReal || fieldType == OFTInteger || fieldType == OFTInteger64)
                {
                    mFeature->SetField(fieldIndex, arg);
                }
                else if (fieldType == OFTString)
                {
                    mFeature->SetField(fieldIndex, std::to_string(arg).c_str());
                }
                else
                {
                    throw std::runtime_error("Type mismatch for double value");
                }
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                if (fieldType == OFTString)
                {
                    VRSA_DEBUG("VectorFeature", arg);
                    //qDebug()<< QString::fromStdString(arg);
                    mFeature->SetField(fieldIndex, arg.c_str());
                }
                else if (fieldType == OFTInteger || fieldType == OFTInteger64)
                {
                    // Пытаемся преобразовать строку в число
                    try {
                        int intValue = std::stoi(arg);
                        mFeature->SetField(fieldIndex, intValue);
                    } catch (...) {
                        throw std::runtime_error("Cannot convert string to integer");
                    }
                }
                else if (fieldType == OFTReal)
                {
                    try {
                        double doubleValue = std::stod(arg);
                        mFeature->SetField(fieldIndex, doubleValue);
                    } catch (...) {
                        throw std::runtime_error("Cannot convert string to double");
                    }
                }
                else
                {
                    mFeature->SetField(fieldIndex, arg.c_str());
                }
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                if (fieldType == OFTInteger || fieldType == OFTInteger64)
                {
                    mFeature->SetField(fieldIndex, arg ? 1 : 0);
                }
                else if (fieldType == OFTString)
                {
                    mFeature->SetField(fieldIndex, arg ? "true" : "false");
                }
                else if (fieldType == OFTReal)
                {
                    mFeature->SetField(fieldIndex, arg ? 1.0 : 0.0);
                }
                else
                {
                    throw std::runtime_error("Type mismatch for boolean value");
                }
            }
        }, value);

    } catch (const std::exception& e) {
        VRSA_ERROR("VectorFeature", "Failed to update attribute" + name +" :" + e.what());
        return false;
    }

    mAttributes[name] = value;
    mParentLayer->SetFeature(mFeature.get());
    emit attributesChanged(name, value);
    emit featureChanged();


    return true;
}

std::unique_ptr<vrsa::vector::VectorFeature> vrsa::vector::VectorFeature::createFeature(VectorLayer *layer)
{
    if (!layer)
    {
        VRSA_ERROR("VectorFeature", "Can't create feature: passed VectorLayer nullptr");
        return nullptr;
    }
    OGRLayer* ogrL = layer->getOGRLayer();
    if (!ogrL)
    {
        VRSA_ERROR("VectorFeature", "Can't create feature: passed VectorLayer has nullptr OGRLayer");
        return nullptr;
    }
    OGRFeature* feature = OGRFeature::CreateFeature(ogrL->GetLayerDefn());
    vrsa::gdalwrapper::OgrFeaturePtr ptrFeat(feature);
    return std::make_unique<VectorFeature>(std::move(ptrFeat), ogrL);
}

void vrsa::vector::VectorFeature::setGeometry(const GeometryVariant &geometry)
{
    std::visit([this](const auto& geom) {
        using T = std::decay_t<decltype(geom)>;

        if constexpr (std::is_same_v<T, PointType>) {
            setPointGeometry(geom);
        }
        else if constexpr (std::is_same_v<T, MultiPointType>) {
            setMultiPointGeometry(geom);
        }
        else if constexpr (std::is_same_v<T, LineStringType>) {
            setLineStringGeometry(geom);
        }
        else if constexpr (std::is_same_v<T, MultiLineStringType>) {
            setMultiLineStringGeometry(geom);
        }
        else if constexpr (std::is_same_v<T, PolygonType>) {
            setPolygonGeometry(geom, {});
        }
        else if constexpr (std::is_same_v<T, PolygonWithHolesType>) {
            setPolygonGeometry(geom.first, geom.second);
        }
        else if constexpr (std::is_same_v<T, MultiPolygonType>) {
            setMultiPolygonGeometry(geom);
        }
    }, geometry);
}

void vrsa::vector::VectorFeature::setPointGeometry(const QPointF &p)
{
    OGRPoint point(p.x(), p.y());
    mFeature->SetGeometry(&point);
}

void vrsa::vector::VectorFeature::setMultiPointGeometry(const std::vector<QPointF> &points)
{
    OGRMultiPoint multiPoint;
    for (const auto& p : points)
    {
        OGRPoint point (p.x(), p.y());
        multiPoint.addGeometry(&point);
    }
    mFeature->SetGeometry(&multiPoint);
}

void vrsa::vector::VectorFeature::setLineStringGeometry(const std::vector<QPointF> &points)
{
    OGRLineString line;
    for (const auto& p : points)
    {
        line.addPoint(p.x(), p.y());
    }
    mFeature->SetGeometry(&line);
}

void vrsa::vector::VectorFeature::setMultiLineStringGeometry(const std::vector<std::vector<QPointF> > &lines)
{
    OGRMultiLineString multiLine;
    for (const auto& linePoints : lines)
    {
        OGRLineString line;
        for (const auto& p : linePoints)
        {
            line.addPoint(p.x(), p.y());
        }
        multiLine.addGeometry(&line);
    }
    mFeature->SetGeometry(&multiLine);
}

void vrsa::vector::VectorFeature::setPolygonGeometry(const std::vector<QPointF> &exterior, const std::vector<std::vector<QPointF> > &holes)
{
    OGRPolygon polygon;

    // внешнее кольцо
    OGRLinearRing extRing;
    for (const auto& p : exterior)
    {
        extRing.addPoint(p.x(), p.y());
    }
    extRing.closeRings();
    polygon.addRing(&extRing);

    // отверстия
    for (const auto& hole : holes)
    {
        OGRLinearRing intRing;
        for (const auto& p : hole)
        {
            intRing.addPoint(p.x(), p.y());
        }
        intRing.closeRings();
        polygon.addRing(&intRing);
    }

    mFeature->SetGeometry(&polygon);
}

void vrsa::vector::VectorFeature::setMultiPolygonGeometry(const std::vector<std::vector<std::vector<QPointF> > > &polygons)
{
    OGRMultiPolygon multiPolygon;

    for (const auto& polygonData : polygons)
    {
        OGRPolygon polygon;

        // Первый вектор - внешнее кольцо
        if (!polygonData.empty()) {
            OGRLinearRing extRing;
            for (const auto& p : polygonData[0])
            {
                extRing.addPoint(p.x(), p.y());
            }
            extRing.closeRings();
            polygon.addRing(&extRing);

            // Остальные - отверстия
            for (size_t i = 1; i < polygonData.size(); ++i)
            {
                OGRLinearRing intRing;
                for (const auto& p : polygonData[i])
                {
                    intRing.addPoint(p.x(), p.y());
                }
                intRing.closeRings();
                polygon.addRing(&intRing);
            }
        }

        multiPolygon.addGeometry(&polygon);
    }

    mFeature->SetGeometry(&multiPolygon);
}


