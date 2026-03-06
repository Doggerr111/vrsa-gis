#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "common/GisDefines.h"
#include <QPointF>
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
}
}
#endif // GEOMETRY_H
