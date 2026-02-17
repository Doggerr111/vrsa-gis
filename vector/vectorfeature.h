#ifndef VECTORFEATURE_H
#define VECTORFEATURE_H
#include <QObject>
//#include <ogr_geometry.h>
#include "gdal/gdalresourcehandles.h"
#include <unordered_map>
#include <variant>
#include "gdal/geometrytypeconverter.h"
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

public:
    VectorFeature(vrsa::gdalwrapper::OgrFeaturePtr feature, OGRLayer* layer = nullptr);

    void setGeometry(vrsa::gdalwrapper::OgrGeometryPtr ptr);
    void setName(const std::string& name);
    void setAttribute(const std::string& name, const AttributeValue& value);
    void setVisible(bool visible);

    vrsa::common::GeometryType getType() const;
    OGRGeometry *getOGRGeometry() const;
    AttributeValue getAttribute(const std::string& name) const;
    OGRFeature* getOGRFeature() const;

    bool updateAttribute(const std::string& name, const AttributeValue& value);


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


    using GeometryVariant = std::variant<
        PointType,                    // точка
        MultiPointType,              // мультиточка
        LineStringType,             // линия
        MultiLineStringType,        // мультилиния
        PolygonType,                // полигон
        PolygonWithHolesType,       // полигон с отверстиями
        MultiPolygonType           // мультиполигон
    >;


    void setGeometry(const GeometryVariant& geometry);

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
