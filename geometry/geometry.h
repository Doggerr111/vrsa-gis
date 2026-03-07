#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "common/GisDefines.h"
#include <QPointF>
#include <QRectF>
namespace vrsa
{
namespace geometry
{
using GeometryVariant = std::variant<
    QPointF,
    std::vector<QPointF>,
    std::vector<std::vector<QPointF>>
    >;

struct Geometry {
    common::GeometryType type;
    GeometryVariant variant;
};

inline Geometry rectToGeometry(const QRectF& rectF)
{
    std::vector<QPointF> vec;
    vec.reserve(4);
    vec.push_back(rectF.topLeft());
    vec.push_back(rectF.topRight());
    vec.push_back(rectF.bottomRight());
    vec.push_back(rectF.bottomLeft());
    //замкнуть???
    return Geometry{common::GeometryType::Polygon, std::move(vec)};
}
inline Geometry rectToGeometry(QRect rect)
{
    std::vector<QPointF> vec;
    vec.reserve(4);
    vec.push_back(rect.topLeft());
    vec.push_back(rect.topRight());
    vec.push_back(rect.bottomRight());
    vec.push_back(rect.bottomLeft());
    //замкнуть???
    return Geometry{common::GeometryType::Polygon, std::move(vec)};
};

}
}
#endif // GEOMETRY_H
