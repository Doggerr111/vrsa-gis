#ifndef GEOMCALCULATIONS_H
#define GEOMCALCULATIONS_H
#include <geometry/geometry.h>
namespace vrsa
{
namespace calculations
{
    QPointF midPoint(const QPointF& p1, const QPointF& p2)
    {
        return QPointF(( p1.x()+p2.x() ) / 2, ( p1.y()+p2.y() ) / 2);
    }
    /**
     * @english
     * @brief Returns geometry::Geometry with GeometryType::Point. Make sure passed geomPoint1 and geomPoint2 have point geometry type
     * @endenglish
     * @russian
     * @brief Возвращает geometry::Geometry c точечным типом геометрии (GeometryType::Point). Убедитесь, что переданные аргументы также
     * обладают точечным типом геометрии
     * @endrussian
     */
    geometry::Geometry midPoint(const geometry::Geometry& geomPoint1, const geometry::Geometry& geomPoint2)
    {
        if (geomPoint1.type != common::GeometryType::Point || geomPoint2.type != common::GeometryType::Point)
            return {};
        const QPointF *point1 = std::get_if<QPointF>(&geomPoint1.variant);
        const QPointF *point2 = std::get_if<QPointF>(&geomPoint2.variant);
        if (!point1 || !point2)
            return {};
        QPointF midPoint = QPointF(( point1->x()+point2->x() ) / 2, ( point1->y()+point2->y() ) / 2);
        geometry::Geometry midPointGeom;
        midPointGeom.type = common::GeometryType::Point;
        midPointGeom.variant = midPoint;
        return midPointGeom;
    }

    /**
     * @english
     *@brief   Creates a new geometry containing midpoints of all segments
     *@details For LineString and Polygon geometries, generates points at the middle of each segment.
     *        Returns a MultiPoint geometry with the same coordinate system.
     *@param  geom Input geometry (LineString or Polygon)
     *@return MultiPoint geometry with midpoints, empty if input type not supported
     * @endenglish
     * @russian
     *@brief   Создает новую геометрию с серединными точками всех сегментов
     *@details Для геометрий типа LineString и Polygon генерирует точки в середине каждого сегмента.
     *         Возвращает геометрию типа MultiPoint с той же системой координат.
     *@param  geom Входная геометрия (LineString или Polygon)
     *@return Геометрия MultiPoint с серединными точками, пустая если тип не поддерживается
     *
     * @endrussian
     */
    geometry::Geometry midPoints(const geometry::Geometry& geomPoint)
    {
        if (geomPoint.type == common::GeometryType::Point)
            return {};
        const std::vector<QPointF>* points = std::get_if<std::vector<QPointF>>(&geomPoint.variant);
        if (!points || points->size() < 2)
            return {};
        std::vector<QPointF> midPoints;
        midPoints.reserve(points->size() - 1);
        for (int i=0; i<points->size()-1; ++i)
        {
            QPointF p1 = points->at(i);
            QPointF p2 = points->at(i+1);
            midPoints.emplace_back(( p1.x()+p2.x() ) / 2, ( p1.y()+p2.y() ) / 2);
        }
        geometry::Geometry midPointsGeom;
        midPointsGeom.type = common::GeometryType::MultiPoint;
        midPointsGeom.variant = std::move(midPoints);
        return midPointsGeom;
    }
}
}

#endif // GEOMCALCULATIONS_H
