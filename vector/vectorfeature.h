#ifndef VECTORFEATURE_H
#define VECTORFEATURE_H
#include <QObject>
//#include <ogr_geometry.h>
#include "gdal/gdalresourcehandles.h"
#include <unordered_map>
#include <variant>
#include "gdal/geometrytypeconverter.h"
#include <QPointF>

#include "geometry/geometry.h"
namespace vrsa
{
namespace vector
{
class VectorLayer;

class VectorFeature :  public QObject
{
    Q_OBJECT

    // Типы данных для атрибутов
public:
    using AttributeValue = std::variant<
    std::string,    // строки
    int,            // целые числа
    double,         // вещественные числа
    bool,           // булевы значения
    std::nullptr_t  // null значения
    >;

private:
    OGRGeometry * mGeometry;
    std::string mName;
    using AttributeMap = std::unordered_map<std::string, AttributeValue>;
    AttributeMap mAttributes;
    vrsa::gdalwrapper::OgrFeaturePtr mFeature;
    //QVariantMap mAttributes;
    bool mVisible;
    OGRLayer* mParentLayer;
    bool mIsSelected;

public:
    VectorFeature(vrsa::gdalwrapper::OgrFeaturePtr feature, OGRLayer* layer = nullptr);

    void setGeometry(vrsa::gdalwrapper::OgrGeometryPtr ptr);
    bool setGeometry(const geometry::Geometry& geometry);
    void setName(const std::string& name);
    void setAttribute(const std::string& name, const AttributeValue& value);
    void setVisible(bool visible);

    vrsa::common::GeometryType getType() const;
    OGRGeometry *getOGRGeometry() const;

    //различные геттеры для атрибутов
    AttributeValue getAttribute(const std::string& name) const;
    QVariant getAttributeAsQVariant(const std::string& name) const;
    std::unordered_map<std::string, std::string> getAttributesAsString() const;     //name - value (string)
    std::unordered_map<QString, QString> getAttributesAsQString() const;     //for ui
    std::string getAttributeAsString(const std::string& name) const;
    QString getAttributeAsQString(const std::string& name) const;
    std::vector<std::string> getAttributeNames() const;
    OGRLayer* getParentOGRLayer() const noexcept { return mParentLayer; };

    OGRFeature* getOGRFeature() const;


    bool updateAttribute(const std::string& name, const AttributeValue& value);
    void setSelected(const bool selected) noexcept { mIsSelected = selected; };

    static std::unique_ptr<vrsa::vector::VectorFeature> createFeature(VectorLayer *layer);
signals:
    void visibilityChanged(bool);
    void featureChanged();
    void attributesChanged(const std::string& name, const AttributeValue& value);


public:

    using PointType = QPointF;
    using LineStringType = std::vector<QPointF>;
    using PolygonType = std::vector<QPointF>;  // exterior ring
    using PolygonWithHolesType = std::pair<std::vector<QPointF>, std::vector<std::vector<QPointF>>>;
    using MultiPointType = std::vector<QPointF>;
    using MultiLineStringType = std::vector<std::vector<QPointF>>;
    using MultiPolygonType = std::vector<std::vector<std::vector<QPointF>>>;


    //09.03
    std::vector<std::string> getFieldNames() const;
    vrsa::common::FieldType getFieldType(const std::string& name) const;
    bool hasField(const std::string& name) const;
    int getFieldCount() const;

    // НОВЫЕ МЕТОДЫ для геометрии
    bool isMultiGeometry() const;
    std::vector<std::unique_ptr<VectorFeature>> explodeToSimpleFeatures() const;
    std::unique_ptr<VectorFeature> clone() const;



    vrsa::common::FieldType determineFieldType(OGRFieldDefn* fieldDefn) const
    {
        switch(fieldDefn->GetType()) {
            case OFTInteger: return vrsa::common::FieldType::Integer;
            case OFTInteger64: return vrsa::common::FieldType::Integer64;
            case OFTReal: return vrsa::common::FieldType::Real;
            case OFTString: return vrsa::common::FieldType::String;
            case OFTDate: return vrsa::common::FieldType::Date;
            case OFTTime: return vrsa::common::FieldType::Time;
            case OFTDateTime: return vrsa::common::FieldType::DateTime;
            case OFTBinary: return vrsa::common::FieldType::Binary;
            default: return vrsa::common::FieldType::Unknown;
        }
    }



    OGRFieldType convertToOGRFieldType(vrsa::common::FieldType type) const {
        switch(type) {
            case vrsa::common::FieldType::Integer: return OFTInteger;
            case vrsa::common::FieldType::Integer64: return OFTInteger64;
            case vrsa::common::FieldType::Real: return OFTReal;
            case vrsa::common::FieldType::String: return OFTString;
            case vrsa::common::FieldType::Date: return OFTDate;
            case vrsa::common::FieldType::Time: return OFTTime;
            case vrsa::common::FieldType::DateTime: return OFTDateTime;
            case vrsa::common::FieldType::Binary: return OFTBinary;
            case vrsa::common::FieldType::Boolean: return OFTInteger; // Boolean как Integer
            default: return OFTString;
        }
    }


    vrsa::common::FieldType determineTypeFromValue(const AttributeValue& value) const {
        if (std::holds_alternative<int>(value)) {
            return vrsa::common::FieldType::Integer;
        } else if (std::holds_alternative<double>(value)) {
            return vrsa::common::FieldType::Real;
        } else if (std::holds_alternative<bool>(value)) {
            return vrsa::common::FieldType::Boolean;
        } else if (std::holds_alternative<std::string>(value)) {
            return vrsa::common::FieldType::String;
        } else {
            return vrsa::common::FieldType::Unknown;
        }
    }


private:
    // Новое поле для хранения информации о типах полей
    std::unordered_map<std::string, vrsa::common::FieldType> mFieldTypes;

    // Метод для определения типа поля из OGR
    //vrsa::common::FieldType determineFieldType(OGRFieldDefn* fieldDefn) const;


//    using GeometryVariant = std::variant<
//        QPointF,
//        std::vector<QPointF>,
//        std::vector<std::vector<QPointF>>
//        >;

//    struct Geometry {
//        common::GeometryType type;
//        GeometryVariant variant;
//    };

    //bool setGeometry(const geometry::Geometry& geometry);

private:
    // ===== РЕАЛИЗАЦИИ ДЛЯ РАЗНЫХ ТИПОВ =====
    void setPointGeometry(const QPointF& p);
    void setMultiPointGeometry(const std::vector<QPointF>& points);
    void setLineStringGeometry(const std::vector<QPointF>& points);
    void setMultiLineStringGeometry(const std::vector<std::vector<QPointF>>& lines);
    void setPolygonGeometry(const std::vector<QPointF>& exterior,
                           const std::vector<std::vector<QPointF>>& holes = {});
    void setMultiPolygonGeometry(const std::vector<std::vector<std::vector<QPointF>>>& polygons);
};

}
}

#endif // VECTORFEATURE_H
