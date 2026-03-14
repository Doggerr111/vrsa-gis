#ifndef TYPECONVERSIONS_H
#define TYPECONVERSIONS_H
#include <GisDefines.h>
#include <ogr_geometry.h>

namespace vrsa
{
namespace gdalwrapper
{

// ===== Конвертация геометрических типов =====

/**
 * @english @brief Converts OGR geometry type to internal GeometryType
 * @russian @brief Преобразует OGR тип геометрии во внутренний GeometryType
*/
inline vrsa::common::GeometryType FromOGR(OGRwkbGeometryType ogrType)
{
    auto flatType = wkbFlatten(ogrType); //2D версия типа геометрии
    if (flatType >= static_cast<int>(vrsa::common::GeometryType::Unknown) &&
            flatType <= static_cast<int>(vrsa::common::GeometryType::Triangle))
        return static_cast<vrsa::common::GeometryType>(flatType);
    return vrsa::common::GeometryType::Unknown;
}

/**
 * @english @brief Converts internal GeometryType to OGR geometry type
 * @russian @brief Преобразует внутренний GeometryType в OGR тип геометрии
*/
inline constexpr OGRwkbGeometryType ToOGR(vrsa::common::GeometryType type)
{
    if (type >= vrsa::common::GeometryType::Unknown && type <= vrsa::common::GeometryType::Triangle)
        return static_cast<OGRwkbGeometryType>(static_cast<int>(type));
    return wkbUnknown;
}

/**
 * @english @brief Checks if OGR geometry type is simple (point/line/polygon) without multi-geometry
 * @russian @brief Проверяет, является ли OGR тип геометрии простым (точка/линия/полигон) без мульти-геометрии
*/
inline bool isSimpleGeometry(OGRwkbGeometryType type)
{
    type = wkbFlatten(type);
    return type == wkbPoint || type == wkbLineString || type == wkbPolygon ||
            type == wkbTriangle || type == wkbCircularString;
}

/**
 * @english @brief Checks if internal GeometryType is simple (point/line/polygon)
 * @russian @brief Проверяет, является ли внутренний тип геометрии простым (точка/линия/полигон)
*/
inline bool isSimpleGeometry(common::GeometryType type) {
    return type == common::GeometryType::Point || type == common::GeometryType::LineString ||
            type == common::GeometryType::Polygon || type == common::GeometryType::Triangle ||
            type == common::GeometryType::CircularString;
}

/**
 * @english @brief Converts multi-geometry to simple geometry type (MultiPoint -> Point)
 * @russian @brief Преобразует мульти-геометрию в простой тип (MultiPoint -> Point)
*/
inline OGRwkbGeometryType getSimpleGeometryType(OGRwkbGeometryType type)
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

/**
 * @english @brief Converts internal multi-geometry to simple geometry type
 * @russian @brief Преобразует внутреннюю мульти-геометрию в простой тип
*/
inline common::GeometryType getSimpleGeometryType(common::GeometryType type)
{
    switch(type)
    {
        case common::GeometryType::MultiPoint: return common::GeometryType::Point;
        case common::GeometryType::MultiLineString: return common::GeometryType::LineString;
        case common::GeometryType::MultiPolygon: return common::GeometryType::Polygon;
        default: return common::GeometryType::Unknown;
    }
}


/**
 * @english @brief Checks if OGR geometry type is valid
 * @russian @brief Проверяет корректность OGR типа геометрии
 */
inline bool IsValidOGRType(OGRwkbGeometryType ogrType)
{
    auto flatType = wkbFlatten(ogrType);
    return flatType >= static_cast<int>(vrsa::common::GeometryType::Unknown) &&
            flatType <= static_cast<int>(vrsa::common::GeometryType::Triangle);
}

/**
 * @english @brief Checks if internal geometry type is valid
 * @russian @brief Проверяет корректность внутреннего типа геометрии
*/
inline constexpr bool IsValidGeometryType(vrsa::common::GeometryType type)
{
    return type >= vrsa::common::GeometryType::Unknown && type <= vrsa::common::GeometryType::Triangle;
}


// ===== Конвертация типов полей аттрибутов =====


/**
 * @english @brief Converts OGR field type to internal FieldType
 * @russian @brief Преобразует OGR тип поля во внутренний FieldType
*/
inline vrsa::common::FieldType convertFromOGRFieldType(OGRFieldType ogrType)
{
    switch(ogrType)
    {
        case OFTInteger: return vrsa::common::FieldType::Integer;
        case OFTInteger64: return vrsa::common::FieldType::Integer64;
        case OFTReal: return vrsa::common::FieldType::Real;
        case OFTString: return vrsa::common::FieldType::String;
        case OFTDate: return vrsa::common::FieldType::Date;
        case OFTTime: return vrsa::common::FieldType::Time;
        case OFTDateTime: return vrsa::common::FieldType::DateTime;
        case OFTBinary: return vrsa::common::FieldType::Binary;
        default: return vrsa::common::FieldType::Unknown;
    }
}


/**
 * @english @brief Converts internal FieldType to OGR field type
 * @russian @brief Преобразует внутренний FieldType в OGR тип поля
*/
inline OGRFieldType convertToOGRFieldType(vrsa::common::FieldType type)
{
    switch(type)
    {
        case vrsa::common::FieldType::Integer: return OFTInteger;
        case vrsa::common::FieldType::Integer64: return OFTInteger64;
        case vrsa::common::FieldType::Real: return OFTReal;
        case vrsa::common::FieldType::String: return OFTString;
        case vrsa::common::FieldType::Date: return OFTDate;
        case vrsa::common::FieldType::Time: return OFTTime;
        case vrsa::common::FieldType::DateTime: return OFTDateTime;
        case vrsa::common::FieldType::Binary: return OFTBinary;
        case vrsa::common::FieldType::Boolean: return OFTInteger; //в OGR нет bool (только как подтип) поэтому int
        default: return OFTString;
    }
}



/**
 * @english @brief Checks if OGR field type is valid
 * @russian @brief Проверяет корректность OGR типа поля
 */
inline constexpr bool IsValidOGRFieldType(OGRFieldType type)
{
    return type == OFTInteger || type == OFTInteger64 ||
           type == OFTReal || type == OFTString ||
           type == OFTDate || type == OFTTime ||
           type == OFTDateTime || type == OFTBinary;
}


} //gdalwrapper
} //vrsa
#endif // TYPECONVERSIONS_H
