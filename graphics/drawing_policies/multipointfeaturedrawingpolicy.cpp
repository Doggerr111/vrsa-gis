#include "multipointfeaturedrawingpolicy.h"
#include "simplepointsymbol.h"

vrsa::graphics::MultiPointFeatureDrawingPolicy::MultiPointFeatureDrawingPolicy(const Symbol *symbol)
    : BasePointDrawingPolicy(symbol) {}

void vrsa::graphics::MultiPointFeatureDrawingPolicy::readGeometry(OGRGeometry *geom) const
{
    if (!geom) return;
    OGRMultiPoint* multiPoint = dynamic_cast<OGRMultiPoint*>(geom);
    if (!multiPoint) return;
    mPoints.clear();
    auto pointsCount = multiPoint->getNumGeometries();
    mPoints.reserve(pointsCount);
    for (int i = 0; i < pointsCount; ++i)
    {
        OGRPoint* point = multiPoint->getGeometryRef(i);
        if (!point) continue;
        mPoints.emplace_back(point->getX(), point->getY());

    }
}

void vrsa::graphics::MultiPointFeatureDrawingPolicy::createPath() const
{
    mCache.path = QPainterPath();
    for (const auto& point: mPoints)
    {
        double x = point.x() + mCache.offsetXScaled;
        double y = point.y() + mCache.offsetYScaled;
        double halfPointSize = mCache.pointSize / 2;
        switch (mSymbol->pointType)
        {
        case common::PointSymbolType::Circle:
        {
            //аппроксимация круга 32-угольником
            const int segments = 32;
            double angleStep = 2 * M_PI / segments;
            QPolygonF polygon;
            for (int i = 0; i < segments; ++i)
            {
                double angle = i * angleStep;
                polygon << QPointF(x + halfPointSize * cos(angle), y + halfPointSize * sin(angle));
            }
            polygon << polygon.first();

            mCache.path.addPolygon(polygon);
            break;

        }
        case common::PointSymbolType::Square:
        {
            mCache.path.addRect(x - halfPointSize, y - halfPointSize,
                         mCache.pointSize, mCache.pointSize);
            break;
        }
        case common::PointSymbolType::Triangle:
        {

            QPointF bottomLeftP(x-halfPointSize, y-halfPointSize);
            QPointF topP(x, y+halfPointSize);
            QPointF bottomRightP(x+halfPointSize, y-halfPointSize);
            mCache.path.moveTo(bottomLeftP);
            mCache.path.lineTo(topP);
            mCache.path.lineTo(bottomRightP);
            mCache.path.closeSubpath();
            break;
        }
        case common::PointSymbolType::Rhombus:
        {
            QPointF point1(x - halfPointSize, y);
            QPointF point2(x, y + halfPointSize);
            QPointF point3(x + halfPointSize, y);
            QPointF point4(x, y - halfPointSize);
            mCache.path.moveTo(point1);
            mCache.path.lineTo(point2);
            mCache.path.lineTo(point3);
            mCache.path.lineTo(point4);
            mCache.path.closeSubpath();
            break;
        }
        case common::PointSymbolType::Cross: // диагональный крест (X)
        {
            mCache.path.moveTo(x - halfPointSize, y - halfPointSize);
            mCache.path.lineTo(x + halfPointSize, y + halfPointSize);
            mCache.path.moveTo(x + halfPointSize, y - halfPointSize);
            mCache.path.lineTo(x - halfPointSize, y + halfPointSize);
            break;
        }

        case common::PointSymbolType::Plus:  // плюс (горизонтальная и вертикальная линии)
        {
            mCache.path.moveTo(x - halfPointSize, y);
            mCache.path.lineTo(x + halfPointSize, y);
            mCache.path.moveTo(x, y - halfPointSize);
            mCache.path.lineTo(x, y + halfPointSize);
            break;
        }

        }
    }

}
