#ifndef SPATIALREFERENCELIBRARY_H
#define SPATIALREFERENCELIBRARY_H
#include <string>
#include <unordered_map>
namespace vrsa
{
namespace georef
{
struct CoordinateSystemInfo {
    int epsgCode;
    std::string name;
    std::string wkt;  // Добавляем WKT для быстрого создания
    std::string proj;

    CoordinateSystemInfo(int code, std::string n, std::string w,
                        std::string prj = "")
        : epsgCode(code), name(std::move(n)), wkt(std::move(w)),
          proj(std::move(prj))
    {}
};


class SpatialReferenceLibrary
{
public:
    // Запрещаем создание экземпляров
    SpatialReferenceLibrary() = delete;
    ~SpatialReferenceLibrary() = delete;
    SpatialReferenceLibrary(const SpatialReferenceLibrary&) = delete;
    SpatialReferenceLibrary& operator=(const SpatialReferenceLibrary&) = delete;


private:
//    LIPCoordinateSystem *WGS84 = new LIPCoordinateSystem();
//    WGS84->setProj("+proj=longlat +datum=WGS84 +no_defs +type=crs");
//    //WGS84->getOGRSpatialRef()->SetWellKnownGeogCS("WGS84");
//    WGS84->setName("WGS 84");
//    mCRSLib.append(WGS84);

//    LIPCoordinateSystem *GSK_2011 = new LIPCoordinateSystem();
//    GSK_2011->setProj("+proj=longlat +ellps=GSK2011 +no_defs +type=crs");
//    GSK_2011->setName("ГСК-2011");
//    mCRSLib.append(GSK_2011);

//    LIPCoordinateSystem *PZ_90_11 = new LIPCoordinateSystem();
//    PZ_90_11->setProj("+proj=geocent +a=6378136 +rf=298.257839303 +units=m +no_defs");
//    PZ_90_11->setName("ПЗ-90.11");
//    mCRSLib.append(PZ_90_11);

//    LIPCoordinateSystem *MSK_50_2 = new LIPCoordinateSystem();
//    MSK_50_2->setProj("+proj=tmerc +lat_0=0 +lon_0=38.48333333333 +k=1 +x_0=2250000 +y_0=-5712900.566 +ellps=krass +towgs84=23.57,-140.95,-79.8,0,0.35,0.79,-0.22 +units=m +no_defs");
//    MSK_50_2->setName("МСК-50 Зона 2");
//    mCRSLib.append(MSK_50_2);

//    LIPCoordinateSystem *Pseudo_Mercator = new LIPCoordinateSystem();
//    Pseudo_Mercator->getOGRSpatialRef()->importFromEPSG(3857);
//    Pseudo_Mercator->setName("Проекция Меркатора");
//    mCRSLib.append(Pseudo_Mercator);
    static inline std::unordered_map<std::string, CoordinateSystemInfo> mCRSMap {
        {"WGS 84",  // ключ
            {        // начало значения CoordinateSystemInfo
                4326,
                "WGS84",
                R"(GEOGCS["WGS 84",
                    DATUM["WGS_1984",
                        SPHEROID["WGS 84",6378137,298.257223563,
                            AUTHORITY["EPSG","7030"]],
                        AUTHORITY["EPSG","6326"]],
                    PRIMEM["Greenwich",0,
                        AUTHORITY["EPSG","8901"]],
                    UNIT["degree",0.0174532925199433,
                        AUTHORITY["EPSG","9122"]],
                    AUTHORITY["EPSG","4326"]])",
                "+proj=longlat +datum=WGS84 +no_defs +type=crs",
            }
        },

        {"ГСК_2011",  // ключ
            {        // начало значения CoordinateSystemInfo
                7682,
                "ГСК_2011",
                R"WKT(GEOGCRS["GSK-2011",
                DATUM["Geodezicheskaya Sistema Koordinat 2011",
                    ELLIPSOID["GSK-2011",6378136.5,298.2564151,
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
                    SCOPE["Geodesy."],
                    AREA["Russian Federation - onshore and offshore."],
                    BBOX[39.87,18.92,85.19,-168.97]],
                ID["EPSG",7682]])WKT",
                "+proj=longlat +ellps=GSK2011 +no_defs +type=crs",
            }
        }

    };

};

}
}

#endif // SPATIALREFERENCELIBRARY_H
