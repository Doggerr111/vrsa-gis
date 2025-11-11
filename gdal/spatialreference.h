#ifndef SPATIALREFERENCE_H
#define SPATIALREFERENCE_H
#include "gdal/gdalresourcehandles.h"
#include "common/GisDefines.h"
namespace vrsa
{
namespace gdalwrapper
{

class SpatialReference
{
public:
    SpatialReference();
    SpatialReference(std::string str, vrsa::common::CrsFormat format);
    SpatialReference(int epsgCode);
    SpatialReference(const SpatialReference& other);
    SpatialReference& operator=(const SpatialReference& other);


    static SpatialReference fromWKT(const std::string& wkt);
    static SpatialReference fromEPSG(int epsgCode);
    static SpatialReference fromProj(const std::string& projString);
    static SpatialReference fromUTM(int zone, bool northernHemisphere = true);

    inline const gdalwrapper::OgrSpatialRefPtr& GetOGRSpatialRef() const noexcept
    {
        return mCrs;
    }

    SpatialReference(SpatialReference&& other) noexcept = default;
    SpatialReference& operator=(SpatialReference&& other) noexcept = default;

private:
    vrsa::gdalwrapper::OgrSpatialRefPtr mCrs;

};

}
}

#endif // SPATIALREFERENCE_H
