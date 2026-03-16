#include "multipolygonfeaturedrawingpolicy.h"


vrsa::graphics::MultiPolygonFeatureDrawingPolicy::MultiPolygonFeatureDrawingPolicy(const Symbol *symbol)
    : BasePolygonDrawingPolicy(symbol)
{

}

QPainterPath vrsa::graphics::MultiPolygonFeatureDrawingPolicy::createPathFromOGRGeometry(OGRGeometry *geom) const
{
    QPainterPath path;
    if (!geom) return path;
    OGRMultiPolygon* multiPoly = dynamic_cast<OGRMultiPolygon*>(geom);
    if (!multiPoly) return path;
    //cобираем все точки
    for (int polyIdx = 0; polyIdx < multiPoly->getNumGeometries(); ++polyIdx)
    {
        OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(multiPoly->getGeometryRef(polyIdx));
        if (!polygon) continue;
        OGRLinearRing* exterior = polygon->getExteriorRing();
        if (!exterior) continue;
        if (exterior->getNumPoints() > 0)
        {
            path.moveTo(exterior->getX(0), exterior->getY(0));
            for (int ptIdx = 1; ptIdx < exterior->getNumPoints(); ++ptIdx)
                path.lineTo(exterior->getX(ptIdx), exterior->getY(ptIdx));
            path.closeSubpath();
        }

        //внутренние кольца
        for (int ringIdx = 0; ringIdx < polygon->getNumInteriorRings(); ++ringIdx)
        {
            OGRLinearRing* interior = polygon->getInteriorRing(ringIdx);
            if (!interior) continue;
            if (interior->getNumPoints() > 0)
            {
                path.moveTo(interior->getX(0), interior->getY(0));
                for (int ptIdx = 1; ptIdx < interior->getNumPoints(); ++ptIdx)
                    path.lineTo(interior->getX(ptIdx), interior->getY(ptIdx));
                path.closeSubpath();
            }
        }
    }
    return path;
}
