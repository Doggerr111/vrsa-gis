#include "ogrconverter.h"
#include "QDebug"


vrsa::gdalwrapper::OgrGeometryPtr vrsa::ogr_utils::OGRConverter::toOGR_uniquePTR(const geometry::Geometry &geom)
{
    switch (geom.type)
    {
    case common::GeometryType::Point:
    {
        auto pointVariant = std::get_if<QPointF>(&geom.variant);
        OGRPoint* point = new OGRPoint();
        point->setX(pointVariant->x());
        point->setY(pointVariant->y());
        return gdalwrapper::OgrGeometryPtr(point);
    }
    case common::GeometryType::MultiPoint:
    {
        break;
    }
    case common::GeometryType::LineString:
    {
        auto lineVariant = std::get_if<std::vector<QPointF>>(&geom.variant);
        if (!lineVariant) return nullptr;
        //VRSA_DEBUG("OGR CONVERTER", "LINE VARIANT NOT NULL");
        OGRLineString* line = new OGRLineString();
        for (const auto& pt : *lineVariant)
        {
            //VRSA_DEBUG("OGR CONVERTER", "ADDING POINT");
            //qDebug()<< pt.x() << " " << pt.y();
            line->addPoint(pt.x(), pt.y());
        }
        return gdalwrapper::OgrGeometryPtr(line);
    }
    case common::GeometryType::MultiLineString:
    {
        break;
    }
    case common::GeometryType::Polygon:
    {
        auto polyVariant = std::get_if<std::vector<QPointF>>(&geom.variant);
        if (!polyVariant) return nullptr;

        OGRPolygon* polygon = new OGRPolygon();
        OGRLinearRing* ring = new OGRLinearRing();

        for (const auto& pt : *polyVariant)
            ring->addPoint(pt.x(), pt.y());
        ring->closeRings();

        polygon->addRing(ring);
        OGRGeometryFactory::destroyGeometry(ring);  // addRing создает копию
        return gdalwrapper::OgrGeometryPtr(polygon);
    }
    case common::GeometryType::MultiPolygon:
    {
        break;
    }
    default:
        break;
    }
    return nullptr;
}

OGRGeometry *vrsa::ogr_utils::OGRConverter::toOGR(const geometry::Geometry &geom)
{
    switch (geom.type)
    {
    case common::GeometryType::Point:
    {
        auto pointVariant = std::get_if<QPointF>(&geom.variant);
        OGRPoint* point = new OGRPoint();
        point->setX(pointVariant->x());
        point->setY(pointVariant->y());
        return point;
    }
    case common::GeometryType::MultiPoint:
    {
        break;
    }
    case common::GeometryType::LineString:
    {
        auto lineVariant = std::get_if<std::vector<QPointF>>(&geom.variant);
        if (!lineVariant) return nullptr;

        OGRLineString* line = new OGRLineString();
        for (const auto& pt : *lineVariant)
            line->addPoint(pt.x(), pt.y());
        return line;
    }
    case common::GeometryType::MultiLineString:
    {
        break;
    }
    case common::GeometryType::Polygon:
    {
        auto polyVariant = std::get_if<std::vector<QPointF>>(&geom.variant);
        if (!polyVariant) return nullptr;

        OGRPolygon* polygon = new OGRPolygon();
        OGRLinearRing* ring = new OGRLinearRing();

        for (const auto& pt : *polyVariant)
            ring->addPoint(pt.x(), pt.y());
        ring->closeRings();

        polygon->addRing(ring);
        OGRGeometryFactory::destroyGeometry(ring);  // addRing создает копию
        return polygon;
    }
    case common::GeometryType::MultiPolygon:
    {
        break;
    }
    default:
        break;
    }
    return nullptr;
}
