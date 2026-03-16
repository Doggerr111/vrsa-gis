#include "multilinefeaturedrawingpolicy.h"

vrsa::graphics::MultiLineFeatureDrawingPolicy::MultiLineFeatureDrawingPolicy(const Symbol *symbol)
    : BaseLineDrawingPolicy(symbol){}


QPainterPath vrsa::graphics::MultiLineFeatureDrawingPolicy::createPathFromOGRGeometry(OGRGeometry *geom) const
{
    QPainterPath path;
    if (!geom)
        return path;
    OGRMultiLineString* multiLine = dynamic_cast<OGRMultiLineString*>(geom);
    if (!multiLine)
        return path;
    for (int i = 0; i < multiLine->getNumGeometries(); ++i)
    {
        OGRLineString* line = dynamic_cast<OGRLineString*>(multiLine->getGeometryRef(i));
        if (!line) continue;
        for (int j = 0; j < line->getNumPoints(); ++j)
        {
            QPointF pt(line->getX(j), line->getY(j));
            if (j == 0)
                path.moveTo(pt);
            else
                path.lineTo(pt);
        }
    }
    return path;
}
