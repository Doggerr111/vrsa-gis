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

enum class GeometryType{
    Point,
    LineString,
    Polygon,
    Unknown
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
