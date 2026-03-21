#include "geometryconverter.h"
#include "vector/vectorlayer.h"

std::unique_ptr<geos::geom::Geometry> vrsa::geometry::GeometryConverter::createGEOSPointCollectionFromVectorLayer
                                                                                      (vector::VectorLayer *layer)
{
    if (!layer) return nullptr;

    std::vector<std::unique_ptr<geos::geom::Point>> geosPoints;
    const auto& factory = *geos::geom::GeometryFactory::getDefaultInstance();

    auto processGeometry = [&](const OGRGeometry* geom, auto&& self) -> void
    {
        if (!geom) return;

        OGRwkbGeometryType geomType = wkbFlatten(geom->getGeometryType());

        switch (geomType)
        {
        case wkbPoint:
        {
            auto* point = dynamic_cast<const OGRPoint*>(geom);
            if (point)
            {
                geosPoints.emplace_back(factory.createPoint
                                        (geos::geom::Coordinate(point->getX(), point->getY())));
            }
            break;
        }

        case wkbLineString:
        {
            auto* line = dynamic_cast<const OGRLineString*>(geom);
            if (line)
            {
                for (int i = 0; i < line->getNumPoints(); ++i)
                {
                    geosPoints.emplace_back(factory.createPoint(
                                                geos::geom::Coordinate(line->getX(i), line->getY(i))));
                }
            }
            break;
        }

        case wkbPolygon:
        {
            auto* poly = dynamic_cast<const OGRPolygon*>(geom);
            if (poly)
            {
                const OGRLinearRing* exterior = poly->getExteriorRing();
                if (exterior)
                {
                    for (int i = 0; i < exterior->getNumPoints(); ++i)
                    {
                        geosPoints.emplace_back(factory.createPoint(
                                                    geos::geom::Coordinate(exterior->getX(i), exterior->getY(i))
                                                    ));
                    }
                }
                for (int i = 0; i < poly->getNumInteriorRings(); ++i)
                {
                    const OGRLinearRing* interior = poly->getInteriorRing(i);
                    if (interior)
                    {
                        for (int j = 0; j < interior->getNumPoints(); ++j)
                        {
                            geosPoints.emplace_back(factory.createPoint(
                                                        geos::geom::Coordinate(interior->getX(j), interior->getY(j))));
                        }
                    }
                }
            }
            break;
        }

        case wkbMultiPoint:
        case wkbMultiLineString:
        case wkbMultiPolygon:
        {
            auto* multi = dynamic_cast<const OGRGeometryCollection*>(geom);
            if (multi)
            {
                for (int i = 0; i < multi->getNumGeometries(); ++i)
                    self(multi->getGeometryRef(i), self);  //рекурсия!
            }
            break;
        }

        default:
            VRSA_WARNING("GEOS", "Unsupported geometry type: " + std::to_string(geomType));
            break;
        }
    };

    for (const auto& feature : layer->getFeatures())
    {
        auto* ogrGeom = feature->getOGRGeometry();
        if (ogrGeom && !ogrGeom->IsEmpty())
            processGeometry(ogrGeom, processGeometry);
    }

    if (geosPoints.empty())
    {
        VRSA_WARNING("GEOS", "No points extracted from layer");
        return nullptr;
    }

    VRSA_INFO("GEOS", "Extracted " + std::to_string(geosPoints.size()) + " points");

    return factory.createMultiPoint(std::move(geosPoints));
}
