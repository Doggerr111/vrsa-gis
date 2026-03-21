#ifndef SPATIALOPERATION_DTO_H
#define SPATIALOPERATION_DTO_H
#include <QMetaType>

namespace vrsa
{
namespace common
{
/**
 * @english
 * @brief Data Transfer Object for geos spatial operations
 * @russian
 * @brief DTO для параметров пространственной операции в геос
 *
 * Используется для передачи данных между UI и контроллером.
 */
struct SpatialOperationDTO
{
    std::string firstInputLayerName;
    std::string outputPath;
    //параметры для буфера
    double distance = 10.0;         // радиус (м)
    int quadrantSegments = 8;       // гладкость (8-12 обычно)
    int endCapStyle = 1;            // 1=Round, 2=Flat, 3=Square
    int joinStyle = 1;              // 1=Round, 2=Mitre, 3=Bevel

    //триангуляция/полигоны вороного
    double tolerance = 0.0;

    bool isValid()
    {
        if (firstInputLayerName.empty()) return false;
        if (outputPath.empty()) return false;
        if (quadrantSegments < 1 && endCapStyle < 1 && endCapStyle > 3 && joinStyle < 1 && joinStyle > 3)
            return false;
        if (tolerance<0.0) return false;
        return true;
    }
};
}
}
//для передачи через сигнал-слот
Q_DECLARE_METATYPE(vrsa::common::SpatialOperationDTO)
#endif // SPATIALOPERATION_DTO_H
