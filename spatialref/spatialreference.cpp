#include "spatialreference.h"
#include "common/GisDefines.h"
#include "logger.h"
#include "coordinatetransformer.h"
vrsa::spatialref::SpatialReference::SpatialReference(gdalwrapper::OgrSpatialRefPtr ogrSpatialRef)
    : mCrs{std::move(ogrSpatialRef)}
{

}

vrsa::spatialref::SpatialReference::SpatialReference(OGRSpatialReference *ogrSpatialRef)
    : mCrs{gdalwrapper::OgrSpatialRefPtr(ogrSpatialRef)}
{

}

vrsa::spatialref::SpatialReference::SpatialReference(std::string str, common::CRSDesctiptionFormat format)
    : mCrs{new OGRSpatialReference}
{
    if (!mCrs)
    {
        VRSA_ERROR("PROJ", "Failed to allocate OGRSpatialReference");
        return;
    }

    switch (format)
    {
    case common::CRSDesctiptionFormat::WKT:
        if (mCrs->importFromWkt(str.c_str()) != OGRERR_NONE)
            VRSA_LOG_GDAL_ERROR("PROJ", "Creation of Spatial Reference from WKT:" + str + " failed");
        break;
    case common::CRSDesctiptionFormat::Proj:
        if (mCrs->importFromProj4(str.c_str()) != OGRERR_NONE)
            VRSA_LOG_GDAL_ERROR("PROJ", "Creation of Spatial Reference from Proj4:" + str + " failed");
        break;
    case common::CRSDesctiptionFormat::EPSG:
    {
        try
        {
            int value = std::stoi(str);
            if (mCrs->importFromEPSG(value) != OGRERR_NONE)
                VRSA_ERROR("PROJ", "Creation of Spatial Reference from EPSG code:" + str + " failed");
        } catch (const std::exception& e)
        {
            VRSA_ERROR("PROJ", "Creation of Spatial Reference from EPSG code:" + str + " failed");
        }

    }
    }
}



vrsa::spatialref::SpatialReference::SpatialReference(const SpatialReference &other)
    : mCrs(other.GetOGRSpatialRef()
           ? gdalwrapper::OgrSpatialRefPtr(other.GetOGRSpatialRef()->Clone())
           : nullptr)
{
    if (!mCrs && other.GetOGRSpatialRef())
        VRSA_ERROR("PROJ", "Failed to clone spatial reference");

}

vrsa::spatialref::SpatialReference& vrsa::spatialref::SpatialReference::operator=(const SpatialReference &other)
{
    if (this != &other)
    {
        auto otherOGRref = other.GetOGRSpatialRef();
        if (otherOGRref)
            mCrs = gdalwrapper::OgrSpatialRefPtr(otherOGRref->Clone());
        if (!mCrs && otherOGRref)
            VRSA_ERROR("PROJ", "Failed to clone spatial reference");
    }
    return *this;

}

vrsa::spatialref::SpatialReference vrsa::spatialref::SpatialReference::fromWKT(const std::string &wkt)
{
    auto spatialRef = gdalwrapper::OgrSpatialRefPtr(new OGRSpatialReference());
    if (spatialRef->importFromWkt(wkt.c_str()) != OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("PROJ", "Creation of Spatial Reference from WKT:" + wkt + " failed");
        return {};
    }
    return SpatialReference(std::move(spatialRef));
}

vrsa::spatialref::SpatialReference vrsa::spatialref::SpatialReference::fromEPSG(int epsgCode)
{
    auto spatialRef = gdalwrapper::OgrSpatialRefPtr(new OGRSpatialReference());
    if (spatialRef->importFromEPSG(epsgCode) != OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("PROJ", "Creation of Spatial Reference from EPSG:" + std::to_string(epsgCode) + " failed");
        return {};
    }
    return SpatialReference(std::move(spatialRef));
}

vrsa::spatialref::SpatialReference vrsa::spatialref::SpatialReference::fromProj(const std::string &projString)
{
    auto spatialRef = gdalwrapper::OgrSpatialRefPtr(new OGRSpatialReference());
    if (spatialRef->importFromProj4(projString.c_str()) != OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("PROJ", "Creation of Spatial Reference from PROJ:" + projString + " failed");
        return {};
    }
    return SpatialReference(std::move(spatialRef));
}

vrsa::spatialref::SpatialReference vrsa::spatialref::SpatialReference::fromUTM(int zone, bool isNorth)
{

    auto spatialRef = gdalwrapper::OgrSpatialRefPtr(new OGRSpatialReference());

    //устанавливаем WGS84 как географическую основу
    spatialRef->SetWellKnownGeogCS("WGS84");

    //isNorth = true: северное полушарие
    //isNorth = false: южное полушарие (флаг +south)
    spatialRef->SetUTM(zone, isNorth ? 0 : 1);  // В OGR: 0 = север, 1 = юг

    //    return SpatialReference(std::move(spatialRef));

    //    auto spatialRef = gdalwrapper::OgrSpatialRefPtr(new OGRSpatialReference());

    //       std::string proj = "+proj=utm +zone=" + std::to_string(zone) +
    //                          (isNorth ? " +north" : " +south") +
    //                          " +datum=WGS84 +units=m +no_defs";

    //       if (spatialRef->SetFromUserInput(proj.c_str()) != OGRERR_NONE) ///!!
    //       {
    //           VRSA_LOG_GDAL_ERROR("PROJ", "Failed to create UTM from PROJ");
    //           return {};
    //       }

    return SpatialReference(std::move(spatialRef));
}

bool vrsa::spatialref::SpatialReference::isValid() const
{
    if (!mCrs)
    {
        VRSA_WARNING("PROJ", "CRS is null");
        return false;
    }

    OGRErr err = mCrs->Validate();
    if (err != OGRERR_NONE)
    {
        VRSA_LOG_GDAL_ERROR("PROJ", "CRS validation failed with error: " +
                            std::to_string(err));
        return false;
    }

    return true;
}

double vrsa::spatialref::SpatialReference::getLinearUnits(std::string &unitName) const
{
    if (!mCrs)
    {
        VRSA_WARNING("PROJ", "CRS is null");
        unitName = "Unknown";
        return 0.0;
    }
    const char* unit;
    double units = mCrs->GetLinearUnits(&unit);
    //если nullptr
    unitName = unit ? unit : "Unknown";
    return units;
}

std::string vrsa::spatialref::SpatialReference::getAuthorityCode() const
{
    if (!mCrs) return "Unknown";
    const char* code = mCrs->GetAuthorityCode(nullptr);
    return code ? code : "Unknown";

}

std::string vrsa::spatialref::SpatialReference::getAuthorityName() const
{
    if (!mCrs) return "Unknown";
    const char* code = mCrs->GetAuthorityName(nullptr);
    return code ? code : "Unknown";
}

std::string vrsa::spatialref::SpatialReference::toWKT() const
{
    if (!mCrs) return "";
    char* wkt = nullptr;
    OGRErr err = mCrs->exportToWkt(&wkt);
    if (err != OGRERR_NONE || !wkt)
    {
        VRSA_LOG_GDAL_ERROR("PROJ", "Export to wkt failed");
        CPLFree(wkt);  // очищаем при ошибке
        return "";
    }
    //для автоматической очистки
    gdalwrapper::CplStringPtr ptr(wkt);
    std::string result(ptr.get());
    return result;
}

std::string vrsa::spatialref::SpatialReference::toProj() const
{
    if (!mCrs) return "";
    char* proj = nullptr;
    OGRErr err = mCrs->exportToProj4(&proj);
    if (err != OGRERR_NONE || !proj)
    {
        VRSA_LOG_GDAL_ERROR("PROJ", "Export to wkt failed");
        CPLFree(proj);  // очищаем при ошибке
        return "";
    }
    //для автоматической очистки
    gdalwrapper::CplStringPtr ptr(proj);
    std::string result(ptr.get());
    return result;
}

std::string vrsa::spatialref::SpatialReference::toPrettyWKT() const
{
    if (!mCrs) return "";
    char* prettyWkt = nullptr; // :3
    OGRErr err = mCrs->exportToWkt(&prettyWkt);
    if (err != OGRERR_NONE || !prettyWkt)
    {
        VRSA_LOG_GDAL_ERROR("PROJ", "Export to wkt failed");
        CPLFree(prettyWkt);  // очищаем при ошибке
        return "";
    }
    //для автоматической очистки
    gdalwrapper::CplStringPtr ptr(prettyWkt);
    std::string result(ptr.get());
    return result;
}

int vrsa::spatialref::SpatialReference::toEPSG() const
{
    if (!mCrs)
    {
        VRSA_WARNING("PROJ", "CRS is null, cannot get EPSG code");
        return -1;
    }

    //EPSG код напрямую через AuthorityCode
    const char* authCode = mCrs->GetAuthorityCode("EPSG");
    if (!authCode)
    {
        //пробуем через AutoIdentify
        if (mCrs->AutoIdentifyEPSG() == OGRERR_NONE)
        {
            authCode = mCrs->GetAuthorityCode("EPSG");
        }
    }

    if (!authCode)
    {
        VRSA_LOG_GDAL_ERROR("PROJ", "No EPSG code found for this CRS");
        return -1;
    }

    // Конвертируем строку в число
    try
    {
        return std::stoi(authCode);
    }
    catch (const std::exception& e)
    {
        VRSA_ERROR("PROJ", "Failed to convert EPSG code to int: " + std::string(authCode));
        return -1;
    }
}

bool vrsa::spatialref::SpatialReference::isSame(const SpatialReference &other) const
{
    if (!mCrs && !other.mCrs)
    {
        VRSA_WARNING("PROJ", "Both CRS are null, considering them identical");
    }
    if (!mCrs || !other.mCrs) return false;

    return mCrs->IsSame(other.mCrs.get());
}

std::unique_ptr<vrsa::spatialref::CoordinateTransformer> vrsa::spatialref::SpatialReference::createTransformTo
                                                                    (const SpatialReference &target) const
{
    auto targetSpatialRef = target.GetOGRSpatialRef();
    if (!targetSpatialRef || !mCrs)
    {
        VRSA_ERROR("PROJ", "Failed to create coordinate transformation: nullptr source or target CRS");
        return nullptr;
    }
//    auto* rawTransform = OGRCreateCoordinateTransformation(mCrs.get(), targetSpatialRef);
//    if (!rawTransform)
//    {
//        VRSA_ERROR("PROJ", "Failed to create coordinate transformation");
//        return nullptr;
//    }
    return std::make_unique<CoordinateTransformer>(*this, target);
}

