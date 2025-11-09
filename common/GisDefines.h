#ifndef LIPTYPES_H
#define LIPTYPES_H


namespace vrsa
{
namespace common
{
/**
 * @file GisDefines.h
 *
 * @english
 * @brief Enumerations and macros for GIS system
 *
 * Defines core enumerations, constants and macros used throughout the VRSA GIS.
 * These definitions provide type-safe identifiers for geometry types,
 * coordinate systems, attribute types, and common operations.
 * @endenglish
 *
 * @russian
 * @brief Перечисления и макросы для ГИС системы
 *
 * Определяет основные перечисления, константы и макросы, используемые в VRSA GIS.
 * Эти определения предоставляют типобезопасные идентификаторы для типов геометрии,
 * систем координат, типов атрибутов и общих операций.
 * @endrussian
 */
enum class DatasetType {
    Vector,
    Raster,
    Mixed,
    Unknown,
    Empty
};

enum class GeometryType{
    Unknown = 0,          // Неизвестный тип
    Point = 1,            // Точка
    LineString = 2,       // Линия
    Polygon = 3,          // Полигон
    MultiPoint = 4,       // Мультиточка
    MultiLineString = 5,  // Мультилиния
    MultiPolygon = 6,     // Мультиполигон
    GeometryCollection = 7, // Коллекция геометрий
    CircularString = 8,   // Круговая строка (CircularString)
    CompoundCurve = 9,    // Составная кривая
    CurvePolygon = 10,    // Криволинейный полигон
    MultiCurve = 11,      // Мультикривая
    MultiSurface = 12,    // Мультиповерхность
    Curve = 13,           // Кривая
    Surface = 14,         // Поверхность
    PolyhedralSurface = 15, // Полиэдральная поверхность
    TIN = 16,             // TIN (триангуляционная сетка)
    Triangle = 17         // Треугольник
};

enum class StyleUnit{
    Millimeters,
    Inches,
    Pixels
};

enum class AttributeType{
    INT32,
    String,
    Real
};

class Types
{
public:
    enum class Crs {};

    enum class Drivers{
        ESRIShapefile
    };

    Types();
};

}
}
#endif // LIPTYPES_H
