#ifndef GEOMETRYTYPECONVERTER_H
#define GEOMETRYTYPECONVERTER_H
#include <GisDefines.h>
#include <ogr_geometry.h>

namespace vrsa
{
namespace gdalwrapper
{

class GeometryTypeConverter
{
public:
    GeometryTypeConverter() = delete;
    // Прямое преобразование через static_cast
    static vrsa::common::GeometryType FromOGR(OGRwkbGeometryType ogrType) {
        auto flatType = wkbFlatten(ogrType);
        if (flatType >= static_cast<int>(vrsa::common::GeometryType::Unknown) &&
                flatType <= static_cast<int>(vrsa::common::GeometryType::Triangle)) {
            return static_cast<vrsa::common::GeometryType>(flatType);
        }
        return vrsa::common::GeometryType::Unknown;
    }

    static constexpr OGRwkbGeometryType ToOGR(vrsa::common::GeometryType type)
    {
        if (type >= vrsa::common::GeometryType::Unknown && type <= vrsa::common::GeometryType::Triangle) {
            return static_cast<OGRwkbGeometryType>(static_cast<int>(type));
        }
        return wkbUnknown;
    }

    static bool isSimpleGeometry(OGRwkbGeometryType type) {
        type = wkbFlatten(type);
        return type == wkbPoint ||
               type == wkbLineString ||
               type == wkbPolygon ||
               type == wkbTriangle ||
               type == wkbCircularString;
    }

    static bool isSimpleGeometry(common::GeometryType type) {
            return type == common::GeometryType::Point ||
                   type == common::GeometryType::LineString ||
                   type == common::GeometryType::Polygon ||
                   type == common::GeometryType::Triangle ||
                   type == common::GeometryType::CircularString;
        }



    static OGRwkbGeometryType getSimpleGeometryType(OGRwkbGeometryType type)
    {
        OGRwkbGeometryType flat = wkbFlatten(type);
        switch(flat)
        {
            case wkbMultiPoint: return wkbPoint;
            case wkbMultiLineString: return wkbLineString;
            case wkbMultiPolygon: return wkbPolygon;
            case wkbGeometryCollection: return wkbUnknown;
            default: return flat;
        }
    }

    static common::GeometryType getSimpleGeometryType(common::GeometryType type)
    {

        switch(type)
        {
            case common::GeometryType::MultiPoint: return common::GeometryType::Point;
            case common::GeometryType::MultiLineString: return common::GeometryType::LineString;
            case common::GeometryType::MultiPolygon: return common::GeometryType::Polygon;
            case common::GeometryType::GeometryCollection: return common::GeometryType::Unknown;
            default: return common::GeometryType::Unknown;
        }
    }

    static OGRFieldType convertToOGRFieldType(vrsa::common::FieldType type) {
        switch(type) {
            case vrsa::common::FieldType::Integer:
                return OFTInteger;

            case vrsa::common::FieldType::Integer64:
                return OFTInteger64;

            case vrsa::common::FieldType::Real:
                return OFTReal;

            case vrsa::common::FieldType::String:
                return OFTString;

            case vrsa::common::FieldType::Date:
                return OFTDate;

            case vrsa::common::FieldType::Time:
                return OFTTime;

            case vrsa::common::FieldType::DateTime:
                return OFTDateTime;

            case vrsa::common::FieldType::Binary:
                return OFTBinary;

            case vrsa::common::FieldType::Boolean:
                // В OGR нет прямого булева типа, используем Integer (0/1)
                return OFTInteger;

            case vrsa::common::FieldType::Unknown:
            default:
                // По умолчанию используем строковый тип
                return OFTString;
        }
    }

//    // Безопасные версии
//    static std::optional<GeometryType> SafeFromOGR(OGRwkbGeometryType ogrType) {
//        auto flatType = wkbFlatten(ogrType);
//        if (flatType >= static_cast<int>(GeometryType::Unknown) &&
//                flatType <= static_cast<int>(GeometryType::Triangle)) {
//            return static_cast<GeometryType>(flatType);
//        }
//        return std::nullopt;
//    }

//    static std::optional<OGRwkbGeometryType> SafeToOGR(GeometryType type) {
//        if (type >= GeometryType::Unknown && type <= GeometryType::Triangle) {
//            return static_cast<OGRwkbGeometryType>(static_cast<int>(type));
//        }
//        return std::nullopt;
//    }

    // Проверки валидности
    static bool IsValidOGRType(OGRwkbGeometryType ogrType) {
        auto flatType = wkbFlatten(ogrType);
        return flatType >= static_cast<int>(vrsa::common::GeometryType::Unknown) &&
                flatType <= static_cast<int>(vrsa::common::GeometryType::Triangle);
    }

    static constexpr bool IsValidGeometryType(vrsa::common::GeometryType type) {
        return type >= vrsa::common::GeometryType::Unknown && type <= vrsa::common::GeometryType::Triangle;
    }
};
}
}
#endif // GEOMETRYTYPECONVERTER_H
