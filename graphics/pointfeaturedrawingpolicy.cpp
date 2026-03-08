#include "pointfeaturedrawingpolicy.h"
#include "simplepointsymbol.h"
vrsa::graphics::PointFeatureDrawingPolicy::PointFeatureDrawingPolicy(const Symbol *symbol)
    : VectorFeatureDrawingPolicy(),
      mX{},
      mY{}
{
    assert(symbol->type() == common::SymbolType::SimplePointSymbol);
    mSymbol = static_cast<const SimplePointSymbol*>(symbol);
}

void vrsa::graphics::PointFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom) const
{
    OGRPoint* point = static_cast<OGRPoint*>(geom);
    mX = point->getX();
    mY = point->getY();
    mCache.isGeomValid = true;
}



void vrsa::graphics::PointFeatureDrawingPolicy::cachePath(const DrawingContext &context) const
{
    switch (mSymbol->pointType)
    {
    case common::PointSymbolType::Circle:
    {
        double pointSize = mSymbol->getPointSize() / context.sceneScale;
        double radius = pointSize / 2;
        double xOffsetScaled = mX + (mSymbol->getXOffSet() / context.sceneScale);
        double yOffsetScaled = mY + (mSymbol->getYOffSet() / context.sceneScale);

        mCache.path = QPainterPath();

        //аппроксимация круга 32-угольником
        const int segments = 32;
        double angleStep = 2 * M_PI / segments;

        QPolygonF polygon;
        for (int i = 0; i < segments; ++i)
        {
            double angle = i * angleStep;
            polygon << QPointF(xOffsetScaled + radius * cos(angle), yOffsetScaled + radius * sin(angle));
        }
        polygon << polygon.first();

        mCache.path.addPolygon(polygon);
        mCache.sceneScale = context.sceneScale;
        break;

        //NOTE addEllipse СЛИШКОМ МЕДЛЕННЫЙ!!!!!
        //            mCache.path.addEllipse(QPointF(mX + xOffsetScaled, mY + yOffsetScaled), radius, radius);

    }
    case common::PointSymbolType::Square:
    {
        double pointSize = mSymbol->getPointSize() / context.sceneScale;
        double halfPointSize = pointSize / 2;
        mCache.path = QPainterPath();
        double xOffsetScaled = mX + (mSymbol->getXOffSet() / context.sceneScale);
        double yOffsetScaled = mY + (mSymbol->getYOffSet() / context.sceneScale);
        mCache.path.addRect(xOffsetScaled - halfPointSize, yOffsetScaled - halfPointSize, pointSize, pointSize);
        mCache.sceneScale = context.sceneScale;
        break;
    }
    case common::PointSymbolType::Triangle:
    {
        double pointSize = mSymbol->getPointSize() / context.sceneScale;
        double halfPointSize = pointSize / 2;
        double xOffsetScaled = mX + (mSymbol->getXOffSet() / context.sceneScale);
        double yOffsetScaled = mY + (mSymbol->getYOffSet() / context.sceneScale);
        QPointF bottomLeftP(xOffsetScaled-halfPointSize, yOffsetScaled-halfPointSize);
        QPointF topP(xOffsetScaled, yOffsetScaled+halfPointSize);
        QPointF bottomRightP(xOffsetScaled+halfPointSize, yOffsetScaled-halfPointSize);
        mCache.path = QPainterPath();
        mCache.path.moveTo(bottomLeftP);
        mCache.path.lineTo(topP);
        mCache.path.lineTo(bottomRightP);
        mCache.path.closeSubpath();
        mCache.sceneScale = context.sceneScale;
        break;
    }
    case common::PointSymbolType::Rhombus:
    {
        double pointSize = mSymbol->getPointSize() / context.sceneScale;
        double halfPointSize = pointSize / 2;
        double xOffsetScaled = mX + (mSymbol->getXOffSet() / context.sceneScale);
        double yOffsetScaled = mY + (mSymbol->getYOffSet() / context.sceneScale);
        QPointF point1(xOffsetScaled - halfPointSize, yOffsetScaled);
        QPointF point2(xOffsetScaled, yOffsetScaled + halfPointSize);
        QPointF point3(xOffsetScaled + halfPointSize, yOffsetScaled);
        QPointF point4(xOffsetScaled, yOffsetScaled - halfPointSize);
        mCache.path = QPainterPath();
        mCache.path.moveTo(point1);
        mCache.path.lineTo(point2);
        mCache.path.lineTo(point3);
        mCache.path.lineTo(point4);
        mCache.path.closeSubpath();
        mCache.sceneScale = context.sceneScale;
        break;
    }
    }
}


void vrsa::graphics::PointFeatureDrawingPolicy::paint(const DrawingContext &context)
{
    context.painter->save();
    if (!mCache.isGeomValid)
    {
        cacheGeometry(context.geom);
        cachePath(context);

    }
    if (mCache.sceneScale != context.sceneScale)
    {
        cachePath(context);
    }

    QBrush brush = mSymbol->brush();
    brush.setTransform(QTransform(context.painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
    QPen pen = mSymbol->pen();
    double width = pen.widthF()/context.sceneScale;
    pen.setWidthF(width);
    context.painter->setPen(pen);
    context.painter->setBrush(brush);
    context.painter->setOpacity(mSymbol->opacity);
    context.painter->drawPath(mCache.path);
    context.painter->rotate(mSymbol->rotation);
//    static int i = 0;
//    QFont font = context.painter->font();
//    font.setPointSizeF(10 / context.sceneScale);
//    context.painter->setFont(font);
//    context.painter->drawText(mX, mY, QString::number(i));
    context.painter->restore();


    //    case CustomImage:
    //    {

    //        painter->drawImage(QRectF(p->x()-(mPointSize/2)/mSceneScale, p->y()-(mPointSize/2)/mSceneScale,
    //                                  (mPointSize)/mSceneScale, (mPointSize)/mSceneScale), *mStyle->getCustomImage());
    //    }

}

vrsa::common::GeometryType vrsa::graphics::PointFeatureDrawingPolicy::getType() const
{
    return vrsa::common::GeometryType::Point;
}


QRectF vrsa::graphics::PointFeatureDrawingPolicy::boundingRect(const DrawingContext& context) const
{
    if (!mCache.isGeomValid)
    {
        cacheGeometry(context.geom);
        cachePath(context);
    }

    if (mCache.isGeomValid && mCache.isBoundingRectValid && context.sceneScale == mCache.sceneScale)
        return mCache.boundingRect;

    double scale = context.sceneScale;
    double pointSize = mSymbol->getPointSize() / scale;
    double halfPointSize = pointSize / 2;

    double penWidth = mSymbol->pen().widthF() / scale;
    double halfPenWidth = penWidth / 2;

    double totalRadius = halfPointSize + halfPenWidth;

    double offsetX = mSymbol->getXOffSet() / scale;
    double offsetY = mSymbol->getYOffSet() / scale;
    double centerX = mX + offsetX;
    double centerY = mY + offsetY;

    double left = centerX - totalRadius;
    double right = centerX + totalRadius;
    double bottom = centerY - totalRadius;  // нижний край (меньше Y)
    double top = centerY + totalRadius;     // верхний край (больше Y)

    return mCache.boundingRect = QRectF(left, bottom, right - left, top - bottom);
}

vrsa::graphics::MultiPointFeatureDrawingPolicy::MultiPointFeatureDrawingPolicy(const Symbol *symbol)
    : VectorFeatureDrawingPolicy()
{
    assert(symbol->type() == common::SymbolType::SimplePointSymbol);
    mSymbol = static_cast<const SimplePointSymbol*>(symbol);
}

void vrsa::graphics::MultiPointFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom) const
{
    OGRMultiPoint* multiPoint = (OGRMultiPoint*)geom;
    int pointCount = multiPoint->getNumGeometries();
    mPoints.reserve(pointCount);
    for (int i = 0; i < pointCount; ++i)
    {
        OGRPoint* point = (OGRPoint*)multiPoint->getGeometryRef(i);
        mPoints.push_back(QPointF(point->getX(), point->getY()));
        mCache.isGeomValid = true;
    }

}
void vrsa::graphics::MultiPointFeatureDrawingPolicy::paint(const DrawingContext &context)
{
    context.painter->save();
    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);
    if (mCache.sceneScale != context.sceneScale) {
        double pointSize = mSymbol->getPointSize();
        mCache.path = QPainterPath();
        double radius = (pointSize / 2.0) / context.sceneScale;  // Масштаб СРАЗУ в расчете!
        for (const auto& point: mPoints)
        {
            double xOffsetScaled = mSymbol->getXOffSet() / context.sceneScale;
            double yOffsetScaled = mSymbol->getYOffSet() / context.sceneScale;
            mCache.path.addEllipse(QPointF(point.x() + xOffsetScaled, point.y() + yOffsetScaled), radius, radius);
        }
        mCache.sceneScale = context.sceneScale;
    }
    QPen pen = mSymbol->pen();
    pen.setWidthF(pen.widthF()/context.sceneScale);
    context.painter->setPen(pen);
    QBrush brush = mSymbol->brush();
    brush.setTransform(QTransform(context.painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
    context.painter->setBrush(brush);
    context.painter->drawPath(mCache.path);
    context.painter->restore();

}

vrsa::common::GeometryType vrsa::graphics::MultiPointFeatureDrawingPolicy::getType() const
{
    return common::GeometryType::MultiPoint;
}



QRectF vrsa::graphics::MultiPointFeatureDrawingPolicy::boundingRect(const DrawingContext &context) const
{
    if (mCache.isGeomValid && context.sceneScale == mCache.sceneScale)
        return mCache.boundingRect;

    OGRMultiPoint* multiPoint = static_cast<OGRMultiPoint*>(context.geom);
    if (multiPoint->getNumGeometries() == 0)
    {
        return mCache.boundingRect = QRectF();
    }

    double scale = context.sceneScale;
    double size = mSymbol->getPointSize() / scale;
    double halfSize = size / 2;
    double offsetX = mSymbol->getXOffSet() / scale;
    double offsetY = mSymbol->getYOffSet() / scale;

    // min/max координаты всех точек
    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();

    for (int i = 0; i < multiPoint->getNumGeometries(); i++)
    {
        OGRPoint* point = static_cast<OGRPoint*>(multiPoint->getGeometryRef(i));
        double x = point->getX() + offsetX;
        double y = point->getY() + offsetY;
        minX = std::min(minX, x - halfSize);
        minY = std::min(minY, y - halfSize);
        maxX = std::max(maxX, x + halfSize);
        maxY = std::max(maxY, y + halfSize);
    }

    return mCache.boundingRect = QRectF(minX, minY, maxX - minX, maxY - minY);
}



