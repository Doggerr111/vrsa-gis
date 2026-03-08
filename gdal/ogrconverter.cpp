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
        {
            ring->addPoint(pt.x(), pt.y());
            //qDebug()<<"OGR CONVERTER - "<< pt.x() << " " << pt.y();
        }

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

vrsa::geometry::Geometry vrsa::ogr_utils::OGRConverter::fromOGR(const OGRGeometry *geom)
{
    if (!geom)
        return {};

    OGRwkbGeometryType ogrType = wkbFlatten(geom->getGeometryType());

    switch (ogrType)
    {
    case wkbPoint:
    {
        const OGRPoint* point = static_cast<const OGRPoint*>(geom);
        geometry::Geometry result;
        result.type = common::GeometryType::Point;
        result.variant = QPointF(point->getX(), point->getY());
        return result;
    }

    case wkbMultiPoint:
    {
        const OGRMultiPoint* multiPoint = static_cast<const OGRMultiPoint*>(geom);
        std::vector<QPointF> points;
        points.reserve(multiPoint->getNumGeometries());

        for (int i = 0; i < multiPoint->getNumGeometries(); ++i) {
            const OGRPoint* point = static_cast<const OGRPoint*>(multiPoint->getGeometryRef(i));
            points.emplace_back(point->getX(), point->getY());
        }

        geometry::Geometry result;
        result.type = common::GeometryType::MultiPoint;
        result.variant = std::move(points);
        return result;
    }

    case wkbLineString:
    {
        const OGRLineString* line = static_cast<const OGRLineString*>(geom);
        std::vector<QPointF> points;
        points.reserve(line->getNumPoints());

        for (int i = 0; i < line->getNumPoints(); ++i) {
            points.emplace_back(line->getX(i), line->getY(i));
        }

        geometry::Geometry result;
        result.type = common::GeometryType::LineString;
        result.variant = std::move(points);
        return result;
    }

    case wkbMultiLineString:
    {
        const OGRMultiLineString* multiLine = static_cast<const OGRMultiLineString*>(geom);
        std::vector<std::vector<QPointF>> lines;
        lines.reserve(multiLine->getNumGeometries());

        for (int i = 0; i < multiLine->getNumGeometries(); ++i) {
            const OGRLineString* line = static_cast<const OGRLineString*>(multiLine->getGeometryRef(i));
            std::vector<QPointF> points;
            points.reserve(line->getNumPoints());

            for (int j = 0; j < line->getNumPoints(); ++j) {
                points.emplace_back(line->getX(j), line->getY(j));
            }
            lines.push_back(std::move(points));
        }

        geometry::Geometry result;
        result.type = common::GeometryType::MultiLineString;
        result.variant = std::move(lines);
        return result;
    }

    case wkbPolygon:
    {
        const OGRPolygon* polygon = static_cast<const OGRPolygon*>(geom);
        const OGRLinearRing* exterior = polygon->getExteriorRing();
        std::vector<QPointF> points;

        if (exterior) {
            points.reserve(exterior->getNumPoints());
            for (int i = 0; i < exterior->getNumPoints(); ++i) {
                points.emplace_back(exterior->getX(i), exterior->getY(i));
            }
        }

        geometry::Geometry result;
        result.type = common::GeometryType::Polygon;
        result.variant = std::move(points);
        return result;
    }

    case wkbMultiPolygon:
    {
        const OGRMultiPolygon* multiPoly = static_cast<const OGRMultiPolygon*>(geom);
        std::vector<std::vector<std::vector<QPointF>>> polygons;
        polygons.reserve(multiPoly->getNumGeometries());

        for (int i = 0; i < multiPoly->getNumGeometries(); ++i) {
            const OGRPolygon* polygon = static_cast<const OGRPolygon*>(multiPoly->getGeometryRef(i));
            std::vector<std::vector<QPointF>> rings;

            // Внешнее кольцо
            const OGRLinearRing* exterior = polygon->getExteriorRing();
            if (exterior) {
                std::vector<QPointF> extPoints;
                extPoints.reserve(exterior->getNumPoints());
                for (int j = 0; j < exterior->getNumPoints(); ++j) {
                    extPoints.emplace_back(exterior->getX(j), exterior->getY(j));
                }
                rings.push_back(std::move(extPoints));
            }

            // Внутренние кольца
            for (int k = 0; k < polygon->getNumInteriorRings(); ++k) {
                const OGRLinearRing* interior = polygon->getInteriorRing(k);
                std::vector<QPointF> intPoints;
                intPoints.reserve(interior->getNumPoints());
                for (int j = 0; j < interior->getNumPoints(); ++j) {
                    intPoints.emplace_back(interior->getX(j), interior->getY(j));
                }
                rings.push_back(std::move(intPoints));
            }

            polygons.push_back(std::move(rings));
        }

        geometry::Geometry result;
        result.type = common::GeometryType::MultiPolygon;
        result.variant = std::move(polygons);
        return result;
    }

    default:
        return {};
    }

}
