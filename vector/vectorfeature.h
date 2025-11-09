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

public:
    VectorFeature(vrsa::gdalwrapper::OgrFeaturePtr feature);
    void setGeometry(vrsa::gdalwrapper::OgrGeometryPtr ptr);
    void setName(const std::string& name);
    void setAttribute(const std::string& name, const AttributeValue& value);
    void setVisible(bool visible);

    vrsa::common::GeometryType getType();
    OGRGeometry *getOGRGeometry();

};
}
}

#endif // VECTORFEATURE_H
