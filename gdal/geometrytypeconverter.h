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

    static constexpr OGRwkbGeometryType ToOGR(vrsa::common::GeometryType type) {
        if (type >= vrsa::common::GeometryType::Unknown && type <= vrsa::common::GeometryType::Triangle) {
            return static_cast<OGRwkbGeometryType>(static_cast<int>(type));
        }
        return wkbUnknown;
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
