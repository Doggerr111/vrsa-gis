#include "spatialreference.h"

vrsa::gdalwrapper::SpatialReference::SpatialReference(std::string wkt)
    : mCrs(new OGRSpatialReference)
{
    auto result = mCrs->importFromWkt(wkt.c_str());
    if (result != OGRERR_NONE)
    {
        VRSA_ERROR("SpatialReference", "Failed to import from WKT: " +wkt + "GDAL Error:" + std::to_string(result));
    }
}


vrsa::gdalwrapper::SpatialReference::SpatialReference(int epsgCode)
    : mCrs(new OGRSpatialReference)
{
    auto result = mCrs->importFromEPSG(epsgCode);
    if (result != OGRERR_NONE)
    {
        VRSA_ERROR("SpatialReference", "Failed to import from EPSG: " + std::to_string(epsgCode)
                   + "GDAL Error:" + std::to_string(result));
    }
}

vrsa::gdalwrapper::SpatialReference::SpatialReference(const SpatialReference &other)
    : mCrs(other.mCrs ? OgrSpatialRefPtr(other.mCrs->Clone()) : nullptr)
{}

vrsa::gdalwrapper::SpatialReference &vrsa::gdalwrapper::SpatialReference::operator=(const SpatialReference &other)
{
    if (this != &other) {
        if (other.mCrs) {
            mCrs = OgrSpatialRefPtr(other.mCrs->Clone());
        } else {
            mCrs.reset();
        }
    }
    return *this;
}
