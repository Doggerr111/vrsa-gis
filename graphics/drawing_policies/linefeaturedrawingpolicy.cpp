#include "linefeaturedrawingpolicy.h"

vrsa::graphics::LineFeatureDrawingPolicy::LineFeatureDrawingPolicy(const Symbol *symbol)
    : BaseLineDrawingPolicy(symbol){}

QPainterPath vrsa::graphics::LineFeatureDrawingPolicy::createPathFromOGRGeometry(OGRGeometry *geom) const
{
    QPainterPath path;
    if (!geom)
        return path;
    OGRLineString* line = (OGRLineString*)geom;
    if (!line)
        return path;
    int pointCount = line->getNumPoints();
    std::vector<QPointF> points;
    points.reserve(pointCount);
    for (int i = 0; i < pointCount; ++i)
    {
        if (i==0)
            path.moveTo(QPointF(line->getX(i), line->getY(i)));
        path.lineTo(QPointF(line->getX(i),line->getY(i)));
    }
    return path;

}








