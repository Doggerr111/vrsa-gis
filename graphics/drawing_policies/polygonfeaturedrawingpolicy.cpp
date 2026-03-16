#include "polygonfeaturedrawingpolicy.h"

vrsa::graphics::PolygonFeatureDrawingPolicy::PolygonFeatureDrawingPolicy(const Symbol *symbol)
    : BasePolygonDrawingPolicy(symbol) {}

QPainterPath vrsa::graphics::PolygonFeatureDrawingPolicy::createPathFromOGRGeometry(OGRGeometry *geom) const
{
    QPainterPath path;
    if (!geom) return path;
    OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geom);
    if (!polygon) return path;
    //внешнее кольцо
    OGRLinearRing* exterior = polygon->getExteriorRing();
    if (!exterior) return path;
    if (exterior->getNumPoints() > 0)
    {
        path.moveTo(exterior->getX(0), exterior->getY(0));
        for (int i = 1; i < exterior->getNumPoints(); ++i)
            path.lineTo(exterior->getX(i), exterior->getY(i));
        path.closeSubpath();
    }
    //внутренние кольца
    for (int i = 0; i < polygon->getNumInteriorRings(); ++i)
    {
        OGRLinearRing* interior = polygon->getInteriorRing(i);
        if (!interior) continue;
        if (interior->getNumPoints() > 0)
        {
            path.moveTo(interior->getX(0), interior->getY(0));
            for (int j = 1; j < interior->getNumPoints(); ++j)
                path.lineTo(interior->getX(j), interior->getY(j));
            path.closeSubpath();
        }
    }
    return path;
}




