#include "spatialrefdatabase.h"
#include "spatialreference.h"

vrsa::spatialref::SpatialReferenceDatabase::CRSInfo vrsa::spatialref::SpatialReferenceDatabase::getCRSInfoByEPSG(int code)
{
    auto crsList = getPredefinedCRSs();
    auto it = std::find_if(crsList.begin(), crsList.end(),
                           [code](const CRSInfo& crs) { return crs.authCode == code; });
    if (it != crsList.end()) return *it;
    throw std::runtime_error("CRS not found");
}



vrsa::spatialref::SpatialReference vrsa::spatialref::SpatialReferenceDatabase::createFromIndex(size_t index) const
{
    if (index >= mCrsDB.size()) {
        throw std::out_of_range("CRS index out of range");
    }
    return createFromCRSInfo(mCrsDB[index]);
}

vrsa::spatialref::SpatialReference vrsa::spatialref::SpatialReferenceDatabase::createFromEPSG(int code) const
{
    auto it = std::find_if(mCrsDB.begin(), mCrsDB.end(),
        [code](const CRSInfo& crs) { return crs.authCode == code; });

    if (it == mCrsDB.end()) {
        throw std::runtime_error("EPSG code " + std::to_string(code) + " not found");
    }

    return createFromCRSInfo(*it);
}

vrsa::spatialref::SpatialReference vrsa::spatialref::SpatialReferenceDatabase::createFromName(const std::string &name) const
{
    auto it = std::find_if(mCrsDB.begin(), mCrsDB.end(),
        [&name](const CRSInfo& crs) { return crs.name == name; });

    if (it == mCrsDB.end()) {
        throw std::runtime_error("CRS with name '" + name + "' not found");
    }

    return createFromCRSInfo(*it);
}

std::vector<vrsa::spatialref::SpatialReference> vrsa::spatialref::SpatialReferenceDatabase::createAll() const
{
    std::vector<SpatialReference> result;
    result.reserve(mCrsDB.size());

    for (const auto& crsInfo : mCrsDB) {
        try {
            result.push_back(createFromCRSInfo(crsInfo));
        } catch (const std::exception& e) {
            VRSA_WARNING("PROJ",
                "Failed to create CRS " + crsInfo.name + ": " + e.what());
            // Можно добавить пустую SpatialReference или пропустить
        }
    }

    return result;
}

void vrsa::spatialref::SpatialReferenceDatabase::addCRS(const CRSInfo &info)
{
    mCrsDB.push_back(info);
}

void vrsa::spatialref::SpatialReferenceDatabase::addUserCRS(CRSInfo &info)
{
    info.isUserDefined = true;
    mCrsDB.push_back(info);
}

void vrsa::spatialref::SpatialReferenceDatabase::addUserCRS(const std::string &name, const std::string &authName,
                                                            const int authCode, const std::string &wkt)
{
    mCrsDB.emplace_back(name, authName, authCode, wkt, true);
}

vrsa::spatialref::SpatialReference vrsa::spatialref::SpatialReferenceDatabase::createFromCRSInfo(const CRSInfo &info) const
{
    SpatialReference reference;
    //пробуем по WKT
    if (!info.wktString.empty())
    {
        reference = SpatialReference::fromWKT(info.wktString);
        if (reference.isValid())
            return reference;
    }

    // по PROJ
    if (!info.projString.empty())
    {
        reference = SpatialReference::fromProj(info.projString);
        if (reference.isValid())
            return reference;
    }

    // Пробуем по EPSG коду
    if (info.authCode > 0)
    {
         reference= SpatialReference::fromEPSG(info.authCode);
         if (reference.isValid())
             return reference;
    }
    if (info.isUserDefined)
        VRSA_ERROR("PROJ", "Can't create SpatialReference for user defined CRS named:"
                   + info.name + " with proj string:" + info.projString + " WKT:" + info.wktString);
    else
        VRSA_ERROR("PROJ", "Can't create SpatialReference for CRS named:"
                   + info.name + " with proj string:" + info.projString + " WKT:" + info.wktString);
    return{};
}
std::vector<vrsa::spatialref::SpatialReferenceDatabase::CRSInfo> vrsa::spatialref::SpatialReferenceDatabase::loadCRSList()
{

    std::vector<CRSInfo> list;
    list.reserve(mDBCapacity);

    //WGS 84
    list.emplace_back(
                "WGS 84",
                "EPSG",
                4326,
                "+proj=longlat +datum=WGS84 +no_defs",
                R"WKT(
                GEOGCS["WGS 84",
                    DATUM["WGS_1984",
                        SPHEROID["WGS 84",6378137,298.257223563,
                            AUTHORITY["EPSG","7030"]],
                        AUTHORITY["EPSG","6326"]],
                    PRIMEM["Greenwich",0,
                        AUTHORITY["EPSG","8901"]],
                    UNIT["degree",0.0174532925199433,
                        AUTHORITY["EPSG","9122"]],
                    AUTHORITY["EPSG","4326"]]
                        )WKT",
                false);


    //PSEUDO-MERCATOR
    list.emplace_back(
                "WGS 84 / Pseudo-Mercator",
                "EPSG",
                3857,
                "+proj=merc +a=6378137 +b=6378137 +lat_ts=0 +lon_0=0 +x_0=0 +y_0=0 +k=1 +units=m +nadgrids=@null +wktext +no_defs",
                R"WKT(
                PROJCS["WGS 84 / Pseudo-Mercator",
                    GEOGCS["WGS 84",
                        DATUM["WGS_1984",
                            SPHEROID["WGS 84",6378137,298.257223563,
                                AUTHORITY["EPSG","7030"]],
                            AUTHORITY["EPSG","6326"]],
                        PRIMEM["Greenwich",0,
                            AUTHORITY["EPSG","8901"]],
                        UNIT["degree",0.0174532925199433,
                            AUTHORITY["EPSG","9122"]],
                        AUTHORITY["EPSG","4326"]],
                    PROJECTION["Mercator_1SP"],
                    PARAMETER["central_meridian",0],
                    PARAMETER["scale_factor",1],
                    PARAMETER["false_easting",0],
                    PARAMETER["false_northing",0],
                    UNIT["metre",1,
                        AUTHORITY["EPSG","9001"]],
                    AXIS["Easting",EAST],
                    AXIS["Northing",NORTH],
                    EXTENSION["PROJ4","+proj=merc +a=6378137 +b=6378137 +lat_ts=0 +lon_0=0 +x_0=0 +y_0=0 +k=1 +units=m +nadgrids=@null +wktext +no_defs"],
                    AUTHORITY["EPSG","3857"]]
                        )WKT",
                false);


//=================ОТЕЧЕСТВЕННЫЕ СИСТЕМЫ КООРДИНАТ И ПРОЕКЦИИ================

    //GKS 2011 в градусах
    list.emplace_back(
                "GSK-2011 (degrees)",
                "EPSG",
                7683,
                "+proj=longlat +ellps=GSK2011 +no_defs +type=crs",
                R"(GEOGCS["GSK-2011",
                DATUM["Geodezicheskaya_Sistema_Koordinat_2011",
                    SPHEROID["GSK-2011",6378136.5,298.2564151,
                        AUTHORITY["EPSG","1025"]],
                    AUTHORITY["EPSG","1159"]],
                PRIMEM["Greenwich",0,
                    AUTHORITY["EPSG","8901"]],
                UNIT["degree",0.0174532925199433,
                    AUTHORITY["EPSG","9122"]],
                AUTHORITY["EPSG","7683"]])",
                false);

    //GKS 2011 в метрах
    list.emplace_back(
                "GSK-2011 (meters)",
                "EPSG",
                7681,
                "+proj=geocent +ellps=GSK2011 +units=m +no_defs +type=crs",
                R"(GEOCCS["GSK-2011",
                DATUM["Geodezicheskaya_Sistema_Koordinat_2011",
                    SPHEROID["GSK-2011",6378136.5,298.2564151,
                        AUTHORITY["EPSG","1025"]],
                    AUTHORITY["EPSG","1159"]],
                PRIMEM["Greenwich",0,
                    AUTHORITY["EPSG","8901"]],
                UNIT["metre",1,
                    AUTHORITY["EPSG","9001"]],
                AXIS["Geocentric X",OTHER],
                AXIS["Geocentric Y",OTHER],
                AXIS["Geocentric Z",NORTH],
                AUTHORITY["EPSG","7681"]])",
                false);

    //GKS-2011 zone 20 в метрах
    list.emplace_back(
                "GSK-2011 / Gauss-Kruger zone 20",
                "EPSG",
                20920,
                "+proj=tmerc +lat_0=0 +lon_0=117 +k=1 +x_0=20500000 +y_0=0 +ellps=GSK2011 "
                "+towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs",
                R"(PROJCS["GSK-2011 / Gauss-Kruger zone 20",
                GEOGCS["GSK-2011",
                    DATUM["Geodezicheskaya_Sistema_Koordinat_2011",
                        SPHEROID["GSK-2011",6378136.5,298.2564151],
                        TOWGS84[0,0,0,0,0,0,0]],
                    PRIMEM["Greenwich",0,
                        AUTHORITY["EPSG","8901"]],
                    UNIT["degree",0.0174532925199433,
                        AUTHORITY["EPSG","9122"]],
                    AUTHORITY["EPSG","7683"]],
                PROJECTION["Transverse_Mercator"],
                PARAMETER["latitude_of_origin",0],
                PARAMETER["central_meridian",117],
                PARAMETER["scale_factor",1],
                PARAMETER["false_easting",20500000],
                PARAMETER["false_northing",0],
                UNIT["metre",1,
                    AUTHORITY["EPSG","9001"]],
                AUTHORITY["EPSG","20920"]])",
                false);


    //PZ-90.11 Geocentric
    list.emplace_back(
                "PZ-90.11 Geocentric",
                "EPSG",
                7679,
                "+proj=geocent +a=6378136 +rf=298.257839303 +units=m +no_defs +type=crs",
                R"WKT_Delimiter( GEOCCS["PZ-90.11",
                DATUM["Parametry_Zemli_1990_11",
                    SPHEROID["PZ-90",6378136,298.257839303,
                        AUTHORITY["EPSG","7054"]],
                    AUTHORITY["EPSG","1158"]],
                PRIMEM["Greenwich",0,
                    AUTHORITY["EPSG","8901"]],
                UNIT["metre",1,
                    AUTHORITY["EPSG","9001"]],
                AXIS["Geocentric X",OTHER],
                AXIS["Geocentric Y",OTHER],
                AXIS["Geocentric Z",NORTH],
                AUTHORITY["EPSG","7679"]])WKT_Delimiter",
                false);


    //PZ-90.11 Ellipsoidal 3D CS
    list.emplace_back(
                "PZ-90.11 Ellipsoidal 3D CS",
                "EPSG",
                7680,
                "+proj=longlat +a=6378136 +rf=298.257839303 +no_defs +type=crs",
                R"WKT_Delimiter( GEOGCRS["PZ-90.11",
                DYNAMIC[
                    FRAMEEPOCH[2010]],
                DATUM["Parametry Zemli 1990.11",
                    ELLIPSOID["PZ-90",6378136,298.257839303,
                        LENGTHUNIT["metre",1]]],
                PRIMEM["Greenwich",0,
                    ANGLEUNIT["degree",0.0174532925199433]],
                CS[ellipsoidal,3],
                    AXIS["geodetic latitude (Lat)",north,
                ORDER[1],
                ANGLEUNIT["degree",0.0174532925199433]],
                AXIS["geodetic longitude (Lon)",east,
                ORDER[2],
                ANGLEUNIT["degree",0.0174532925199433]],
                AXIS["ellipsoidal height (h)",up,
                ORDER[3],
                LENGTHUNIT["metre",1]],
                USAGE[
                SCOPE["Geodesy. Navigation and positioning using Glonass satellite system."],
                AREA["World."],
                BBOX[-90,-180,90,180]],
                ID["EPSG",7680]] )WKT_Delimiter",
                false);
    return list;

}

std::vector<vrsa::spatialref::SpatialReferenceDatabase::Ellipsoid> vrsa::spatialref::SpatialReferenceDatabase::loadEllipsoids()
{
    std::vector<Ellipsoid> ellipsoids;
    ellipsoids.emplace_back("WGS 84", 6378137.0, 298.257223563); //WGS 84
    ellipsoids.emplace_back("GSK-2011", 6378136.5, 298.2564151);  // ГСК-2011
    return ellipsoids;
}
