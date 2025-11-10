#ifndef SPATIALREFERENCE_H
#define SPATIALREFERENCE_H
#include "gdal/gdalresourcehandles.h"

namespace vrsa
{
namespace gdalwrapper
{

class SpatialReference
{
public:
    SpatialReference(std::string wkt);
    SpatialReference(int epsgCode);
    SpatialReference(const SpatialReference& other);
    SpatialReference& operator=(const SpatialReference& other);

    SpatialReference(SpatialReference&& other) noexcept = default;
    SpatialReference& operator=(SpatialReference&& other) noexcept = default;

private:
    vrsa::gdalwrapper::OgrSpatialRefPtr mCrs;
};

}
}

#endif // SPATIALREFERENCE_H
