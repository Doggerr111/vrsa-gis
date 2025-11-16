#include "linefeaturedrawingpolicy.h"




vrsa::graphics::LineFeatureDrawingPolicy::LineFeatureDrawingPolicy(VectorFeatureStyle &style)
    : VectorFeatureDrawingPolicy(style)
{

}

void vrsa::graphics::LineFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom)
{
    mCache.path = QPainterPath();
    OGRLineString* line = (OGRLineString*)geom;
    int pointCount = line->getNumPoints();
    std::vector<QPointF> points;
    points.reserve(pointCount);
    for (int i = 0; i < pointCount; ++i)
    {
        if (i==0)
            mCache.path.moveTo(QPointF(line->getX(i), line->getY(i)));
        mCache.path.lineTo(QPointF(line->getX(i),line->getY(i)));
    }
    mCache.isGeomValid = true;
}

void vrsa::graphics::LineFeatureDrawingPolicy::paint(const DrawingContext &context)
{
    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);

    QPen pen = mStyle.getPen();
    pen.setCapStyle(Qt::SquareCap);
    pen.setWidthF(calculations::UnitConversion::mmToPixels(pen.widthF())/context.sceneScale);
    context.painter->setPen(pen);

    context.painter->drawPath(mCache.path);
    mCache.sceneScale = context.sceneScale;
    //}

//    QVector<QPointF> points;
//    OGRGeometry* poGeometry = context.geom;
//    OGRwkbGeometryType geomType = wkbFlatten(poGeometry->getGeometryType());
//    assert(geomType == wkbLineString);

//    OGRLineString* line = (OGRLineString*)poGeometry;
//    int pointCount = line->getNumPoints();

//    for (int i = 0; i < pointCount; i++)
//    {
//        points.push_back(QPointF(line->getX(i),line->getY(i)));
//    }
//    QPen pen = mStyle.getPen();
//    pen.setCapStyle(Qt::SquareCap);
//    pen.setWidthF(calculations::UnitConversion::mmToPixels(pen.widthF())/context.sceneScale);
//    context.painter->setPen(pen);
//    context.painter->drawPolyline(points);

}

vrsa::common::GeometryType vrsa::graphics::LineFeatureDrawingPolicy::getType() const
{
    return vrsa::common::GeometryType::LineString;
}


QRectF vrsa::graphics::LineFeatureDrawingPolicy::boundingRect(const DrawingContext &context) const
{

    if (!mCache.isGeomValid || mCache.path.isEmpty())
            return QRectF();
    if (mCache.isGeomValid && context.sceneScale == mCache.sceneScale)
            return mCache.boundingRect;

        QRectF rect = mCache.path.boundingRect();
        qreal penWidth = mStyle.getPen().widthF();
        qreal scaledPenWidth = calculations::UnitConversion::mmToPixels(penWidth) / mCache.sceneScale;
        qreal halfPenWidth = scaledPenWidth / 2.0;

        return mCache.boundingRect = QRectF(rect.x() - halfPenWidth,
                      rect.y() - halfPenWidth,
                      rect.width() + scaledPenWidth,
                      rect.height() + scaledPenWidth);



//    //todo function
//    QVector<QPointF> points;
//    OGRGeometry* poGeometry = context.geom;
//    OGRwkbGeometryType geomType = wkbFlatten(poGeometry->getGeometryType());
//    assert(geomType == wkbLineString);

//    OGRLineString* line = (OGRLineString*)poGeometry;
//    int pointCount = line->getNumPoints();

//    for (int i = 0; i < pointCount; i++)
//    {
//        points.push_back(QPointF(line->getX(i),line->getY(i)));
//    }
//    // f
//    QPolygonF polyLine = points;
//    qreal minX = polyLine.first().x();
//    qreal maxX = polyLine.first().x();
//    qreal minY = polyLine.first().y();
//    qreal maxY = polyLine.first().y();

//    for (const QPointF& point : polyLine)
//    {
//        minX = qMin(minX, point.x());
//        maxX = qMax(maxX, point.x());
//        minY = qMin(minY, point.y());
//        maxY = qMax(maxY, point.y());
//    }

//    qreal penWidth = calculations::UnitConversion::mmToPixels(mStyle.getPen().widthF()/context.sceneScale);
//    qreal halfPenWidth = penWidth / 2.0;

//    return QRectF(minX - halfPenWidth, minY - halfPenWidth, maxX - minX + penWidth, maxY - minY + penWidth);
}

vrsa::graphics::MultiLineFeatureDrawingPolicy::MultiLineFeatureDrawingPolicy(VectorFeatureStyle &style)
    : VectorFeatureDrawingPolicy(style)
{

}


void vrsa::graphics::MultiLineFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom)
{

    OGRMultiLineString* multiLine = (OGRMultiLineString*)geom;

    // Собираем все точки
    for (int i = 0; i < multiLine->getNumGeometries(); ++i)
    {
        OGRLineString* line = (OGRLineString*)multiLine->getGeometryRef(i);
        for (int j = 0; j < line->getNumPoints(); ++j)
        {
            if (j==0)
                mCache.path.moveTo(QPointF(line->getX(i), line->getY(i)));
            mCache.path.lineTo(QPointF(line->getX(i),line->getY(i)));
        }
    }


        mCache.isGeomValid = true;
}


void vrsa::graphics::MultiLineFeatureDrawingPolicy::paint(const DrawingContext &context)
{


    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);

    QPen pen = mStyle.getPen();
    pen.setCapStyle(Qt::SquareCap);
    pen.setWidthF(calculations::UnitConversion::mmToPixels(pen.widthF())/context.sceneScale);
    context.painter->setPen(pen);

    context.painter->drawPath(mCache.path);
    mCache.sceneScale = context.sceneScale;



//    QVector<QPointF> points;
//    OGRGeometry* poGeometry = context.geom;
//    OGRwkbGeometryType geomType = wkbFlatten(poGeometry->getGeometryType());
//    assert(geomType == wkbMultiLineString);

//    OGRMultiLineString* multiLine = (OGRMultiLineString*)poGeometry;
//    int lineCount = multiLine->getNumGeometries();

//    QPen pen = mStyle.getPen();
//    pen.setCapStyle(Qt::SquareCap);
//    pen.setWidthF(calculations::UnitConversion::mmToPixels(pen.widthF())/context.sceneScale);
//    context.painter->setPen(pen);

//    // Рисуем каждую линию в мультилинии
//    for (int lineIndex = 0; lineIndex < lineCount; lineIndex++)
//    {
//        OGRLineString* line = (OGRLineString*)multiLine->getGeometryRef(lineIndex);
//        int pointCount = line->getNumPoints();

//        points.clear(); // Очищаем массив точек для каждой новой линии

//        for (int i = 0; i < pointCount; i++)
//        {
//            points.push_back(QPointF(line->getX(i), line->getY(i)));
//        }

//        // Рисуем отдельную линию
//        context.painter->drawPolyline(points);
//    }


}

vrsa::common::GeometryType vrsa::graphics::MultiLineFeatureDrawingPolicy::getType() const
{
    return common::GeometryType::MultiLineString;
}

QRectF vrsa::graphics::MultiLineFeatureDrawingPolicy::boundingRect(const DrawingContext &context) const
{

    if (!mCache.isGeomValid || mCache.path.isEmpty())
            return QRectF();
    if (mCache.isGeomValid && context.sceneScale == mCache.sceneScale)
            return mCache.boundingRect;

        QRectF rect = mCache.path.boundingRect();
        qreal penWidth = mStyle.getPen().widthF();
        qreal scaledPenWidth = calculations::UnitConversion::mmToPixels(penWidth) / mCache.sceneScale;
        qreal halfPenWidth = scaledPenWidth / 2.0;

        return mCache.boundingRect = QRectF(rect.x() - halfPenWidth,
                      rect.y() - halfPenWidth,
                      rect.width() + scaledPenWidth,
                      rect.height() + scaledPenWidth);
}




void vrsa::graphics::LineFeatureDrawingPolicy::rebuildCache(const DrawingContext &context)
{
}

//QPainterPath vrsa::graphics::LineFeatureDrawingPolicy::geometryToPath(const DrawingContext &context) const
//{
//}



void vrsa::graphics::MultiLineFeatureDrawingPolicy::rebuildCache(const DrawingContext &context)
{
}

//QPainterPath vrsa::graphics::MultiLineFeatureDrawingPolicy::geometryToPath(const DrawingContext &context) const
//{
//}







