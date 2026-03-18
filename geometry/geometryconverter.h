#ifndef GEOMETRYCONVERTER_H
#define GEOMETRYCONVERTER_H
#include <ogr_geometry.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/LineString.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <memory>
#include "gdalresourcehandles.h"


namespace vrsa
{
namespace geometry
{
/**
 * @english
 * @brief Converts geometries between OGR (GDAL) and GEOS formats
 *
 * Provides bidirectional conversion with:
 * - Fast direct coordinate conversion for simple types
 * - WKT fallback for complex geometries
 * - Smart pointer management for automatic cleanup
 *
 * @russian
 * @brief Преобразует геометрии между форматами OGR (GDAL) и GEOS
 *
 * Обеспечивает двунаправленную конвертацию с:
 * - Быстрой прямой конвертацией координат для простых типов
 * - WKT запасной вариант для сложных геометрий
 * - Умными указателями для автоматической очистки
 */
class GeometryConverter
{
public:
    static const geos::geom::GeometryFactory& getFactory()
    {
        static auto factory = geos::geom::GeometryFactory::create();
        return *factory;
    }

    // OGR -> GEOS через WKT
    static std::unique_ptr<geos::geom::Geometry> fromOGRViaWKT(const OGRGeometry* ogrGeom)
    {
        if (!ogrGeom) return nullptr;
        char* wkt = nullptr;
        ogrGeom->exportToWkt(&wkt);
        gdalwrapper::CplStringPtr wktPtr(wkt);
        if (!wkt) return nullptr;
        geos::io::WKTReader reader(getFactory());
        auto geosGeom = reader.read(wkt);
        return geosGeom;
    }

    // GEOS -> OGR через WKT
    static OGRGeometry* toOGRViaWKT(const geos::geom::Geometry* geosGeom)
    {
        if (!geosGeom) return nullptr;
        geos::io::WKTWriter writer;
        std::string wkt = writer.write(geosGeom);
        OGRGeometry* ogrGeom = nullptr;
        OGRGeometryFactory::createFromWkt(wkt.c_str(), nullptr, &ogrGeom);
        return ogrGeom;
    }

    // OGR -> GEOS через координаты (для простых типов геометрий). Для сложных fallback на wkt
    static std::unique_ptr<geos::geom::Geometry> createGEOSFromOGR(const OGRGeometry* ogrGeom)
    {
        if (!ogrGeom) return nullptr;

        auto& factory = *geos::geom::GeometryFactory::getDefaultInstance();

        switch (wkbFlatten(ogrGeom->getGeometryType()))
        {
        case wkbPoint:
        {
            auto* point = dynamic_cast<const OGRPoint*>(ogrGeom);
            if (!point) return nullptr;
            return std::unique_ptr<geos::geom::Geometry>
                    (factory.createPoint(geos::geom::Coordinate(point->getX(), point->getY())));
        }
        case wkbLineString:
        {
            auto* line = dynamic_cast<const OGRLineString*>(ogrGeom);
            if (!line) return nullptr;
            //geos::geom::CoordinateSequence::Ptr sequencePtr;
            auto seq = std::make_unique<geos::geom::CoordinateSequence>();
            for (int i = 0; i < line->getNumPoints(); ++i)
                seq->add(line->getX(i), line->getY(i));
            return factory.createLineString(std::move(seq));

        }
        case wkbPolygon:
        {
            auto* poly = dynamic_cast<const OGRPolygon*>(ogrGeom);
            if (!poly) return nullptr;
            const OGRLinearRing* exterior = poly->getExteriorRing();
            if (!exterior) return nullptr;

            auto exteriorSeq = std::make_unique<geos::geom::CoordinateSequence>();
            for (int i = 0; i < exterior->getNumPoints(); ++i)
                exteriorSeq->add(exterior->getX(i), exterior->getY(i));

            //внутренниие кольца
            std::vector<std::unique_ptr<geos::geom::LinearRing>> holes;
            for (int i = 0; i < poly->getNumInteriorRings(); ++i)
            {
                const OGRLinearRing* interior = poly->getInteriorRing(i);
                if (!interior) continue;

                auto interiorSeq = std::make_unique<geos::geom::CoordinateSequence>();
                for (int j = 0; j < interior->getNumPoints(); ++j)
                    interiorSeq->add(interior->getX(j), interior->getY(j));

                holes.push_back(std::unique_ptr<geos::geom::LinearRing>(factory.createLinearRing(std::move(interiorSeq))));
            }

            return std::unique_ptr<geos::geom::Geometry>(factory.createPolygon(std::unique_ptr<geos::geom::LinearRing>(
                                                                                   factory.createLinearRing(std::move(exteriorSeq))), std::move(holes)));
        }
        default: //для мультигеометрий и всего остального используем wkt (медленно)
            return fromOGRViaWKT(ogrGeom);

        }//switch
    }

    // GEOS -> OGR через координаты (для простых типов геометрий). Для сложных fallback на wkt
    static gdalwrapper::OgrGeometryPtr createOGRfromGeos(const geos::geom::Geometry* geosGeom)
    {
        if (!geosGeom) return nullptr;
        switch (geosGeom->getGeometryTypeId())
        {
        case geos::geom::GEOS_POINT:
        {
            auto* point = dynamic_cast<const geos::geom::Point*>(geosGeom);
            if (!point) return nullptr;
            return gdalwrapper::OgrGeometryPtr(new OGRPoint(point->getX(), point->getY()));
        }
        case geos::geom::GEOS_LINESTRING:
        {
            auto* line = dynamic_cast<const geos::geom::LineString*>(geosGeom);
            if (!line) return nullptr;
            OGRLineString* ogrLine = new OGRLineString();
            for (size_t i = 0; i < line->getNumPoints(); ++i)
            {
                const auto& coord = line->getCoordinateN(i);
                ogrLine->addPoint(coord.x, coord.y);
            }
            return gdalwrapper::OgrGeometryPtr(ogrLine);
        }

        case geos::geom::GEOS_POLYGON:
        {
            auto* poly = dynamic_cast<const geos::geom::Polygon*>(geosGeom);
            if (!poly) return nullptr;

            OGRPolygon* ogrPoly = new OGRPolygon();
            const geos::geom::LineString* exterior = poly->getExteriorRing(); //внешнее кольцо
            if (exterior)
            {
                OGRLinearRing* ogrShell = new OGRLinearRing();
                for (size_t i = 0; i < exterior->getNumPoints(); ++i)
                {
                    const auto& coord = exterior->getCoordinateN(i);
                    ogrShell->addPoint(coord.x, coord.y);
                }
                ogrPoly->addRing(ogrShell);
            }

            //внутренние кольца
            for (size_t i = 0; i < poly->getNumInteriorRing(); ++i)
            {
                const geos::geom::LineString* hole = poly->getInteriorRingN(i);
                if (!hole) continue;

                OGRLinearRing* interior = new OGRLinearRing();
                for (size_t j = 0; j < hole->getNumPoints(); ++j)
                {
                    const auto& coord = hole->getCoordinateN(j);
                    interior->addPoint(coord.x, coord.y);
                }
                ogrPoly->addRing(interior);

            }
            return gdalwrapper::OgrGeometryPtr(ogrPoly);
        }
        default:
            return gdalwrapper::OgrGeometryPtr(toOGRViaWKT(geosGeom));
        } // switch
    }

};
}
}
#endif // GEOMETRYCONVERTER_H
