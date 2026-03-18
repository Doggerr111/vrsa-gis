#ifndef COORDINATETRANSFORMER_H
#define COORDINATETRANSFORMER_H
#include "spatialref/spatialreference.h"

namespace vrsa
{
namespace spatialref
{

class CoordinateTransformer {
public:
    CoordinateTransformer(const SpatialReference& src, const SpatialReference& dst);
    ~CoordinateTransformer();

    // Запрещаем копирование
    CoordinateTransformer(const CoordinateTransformer&) = delete;
    CoordinateTransformer& operator=(const CoordinateTransformer&) = delete;

    // Разрешаем перемещение
    CoordinateTransformer(CoordinateTransformer&& other) noexcept;
    CoordinateTransformer& operator=(CoordinateTransformer&& other) noexcept;

    // Проверка, создалась ли трансформация
    bool isValid() const noexcept { return m_transform != nullptr; }
    // Трансформация одной точки
    bool transform(double& x, double& y) const;
    bool transform(double& x, double& y, double* z) const;
    // Трансформация геометрии OGR
    OGRGeometry* transformGeometry(const OGRGeometry* geom) const;

    // Получить обратную трансформацию
    std::unique_ptr<CoordinateTransformer> createInverse() const;

    // Получить строку с описанием (для логов)
    std::string getInfo() const;

private:
    std::unique_ptr<OGRCoordinateTransformation, decltype(&OGRCoordinateTransformation::DestroyCT)> m_transform;
    SpatialReference m_src;
    SpatialReference m_dst;
};

} // namespace spatialref
} // namespace vrsa
#endif // COORDINATETRANSFORMER_H
