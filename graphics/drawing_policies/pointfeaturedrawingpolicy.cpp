#include "pointfeaturedrawingpolicy.h"
#include "simplepointsymbol.h"
vrsa::graphics::PointFeatureDrawingPolicy::PointFeatureDrawingPolicy(const Symbol *symbol)
    : BasePointDrawingPolicy(symbol) {}


void vrsa::graphics::PointFeatureDrawingPolicy::readGeometry(OGRGeometry *geom) const
{
    if (!geom) return;
    OGRPoint* point = dynamic_cast<OGRPoint*>(geom);
    if (!point) return;
    mPoints.clear();
    mPoints.emplace_back(point->getX(), point->getY());
}

void vrsa::graphics::PointFeatureDrawingPolicy::createPath() const
{
    if (mPoints.empty()) return;
    mCache.path = QPainterPath();
    double halfPointSize = mCache.pointSize / 2;
    auto p = mPoints[0];
    double x = p.x() + mCache.offsetXScaled;
    double y = p.y() + mCache.offsetYScaled;
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
        //NOTE addEllipse СЛИШКОМ МЕДЛЕННЫЙ!!!!!
        //            mCache.mCache.path.addEllipse(QPointF(mmX + mXOffsetScaled, mY + yOffsetScaled), radius, radius);

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
    case common::PointSymbolType::Cross: // диагональный крест (mX)
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






