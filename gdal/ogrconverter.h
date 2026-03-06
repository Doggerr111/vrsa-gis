#ifndef OGRCONVERTER_H
#define OGRCONVERTER_H
#include "ogr_geometry.h"
#include "geometry/geometry.h"
#include <gdalresourcehandles.h>
namespace vrsa
{
namespace ogr_utils
{

class OGRConverter
{
public:
    OGRConverter() = default;
    static gdalwrapper::OgrGeometryPtr toOGR_uniquePTR(const geometry::Geometry& geom);
    static OGRGeometry* toOGR(const geometry::Geometry& geom);
};

}
}

#endif // OGRCONVERTER_H
