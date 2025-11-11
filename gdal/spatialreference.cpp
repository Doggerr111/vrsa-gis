#include "spatialreference.h"

vrsa::gdalwrapper::SpatialReference::SpatialReference()
    :mCrs(new OGRSpatialReference)
{

}

vrsa::gdalwrapper::SpatialReference::SpatialReference(std::string crsDefinition, common::CrsFormat format)
    : mCrs(new OGRSpatialReference)
{
    switch (format)
    {

    case common::CrsFormat::WKT:
    {
        auto result = mCrs->importFromWkt(crsDefinition.c_str());
        if (result != OGRERR_NONE)
        {
            VRSA_ERROR("SpatialReference", "Failed to import from WKT: " +crsDefinition + "GDAL Error:" + std::to_string(result));
        }
        break;
    }

    case common::CrsFormat::Proj:
    {
        auto result = mCrs->importFromProj4(crsDefinition.c_str());
        if (result != OGRERR_NONE)
        {
            VRSA_ERROR("SpatialReference", "Failed to import from Proj: " +crsDefinition + "GDAL Error:" + std::to_string(result));
        }
        break;
    }

    case common::CrsFormat::EPSG:
    {
        try
        {
            int number = std::stoi(crsDefinition);
            auto result = mCrs->importFromEPSG(number);
            if (result != OGRERR_NONE)
            {
                VRSA_ERROR("SpatialReference", "Failed to import from EPSG: " + crsDefinition + " GDAL Error:" + std::to_string(result));
            }
        }
        catch (const std::invalid_argument& e)
        {
            VRSA_ERROR("SpatialReference", "Failed to import from EPSG (invalid argument): " + crsDefinition);
        }
        catch (const std::out_of_range& e)
        {
            VRSA_ERROR("SpatialReference", "Failed to import from EPSG (out of range): " + crsDefinition);
        }
        catch (const std::exception& e)
        {
            VRSA_ERROR("SpatialReference", "Failed to import from EPSG: " + std::string(e.what()));
        }
        break;
    }
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

vrsa::gdalwrapper::SpatialReference vrsa::gdalwrapper::SpatialReference::fromWKT(const std::string &wkt)
{
    //return SpatialReference(wkt);
}

