#include "linefeaturedrawingpolicy.h"
#include "simplelinesymbol.h"



vrsa::graphics::LineFeatureDrawingPolicy::LineFeatureDrawingPolicy(const Symbol *symbol)
    : VectorFeatureDrawingPolicy()
{
    assert(symbol->type() == common::SymbolType::SimpleLineSymbol);
    mSymbol = static_cast<const SimpleLineSymbol*>(symbol);
}

void vrsa::graphics::LineFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom) const
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
    context.painter->save();
    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);

    QPen pen = mSymbol->pen();
    pen.setWidthF(pen.widthF()/context.sceneScale);
    context.painter->setPen(pen);
    double offsetX = mSymbol->getXOffSet() / context.sceneScale;
    double offsetY = mSymbol->getYOffSet() / context.sceneScale;
    context.painter->translate(offsetX, offsetY);
    context.painter->drawPath(mCache.path);
    mCache.sceneScale = context.sceneScale;

    context.painter->restore();
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
        cacheGeometry(context.geom);
    if (mCache.isGeomValid && context.sceneScale == mCache.sceneScale)
        return mCache.boundingRect;

    QRectF rect = mCache.path.boundingRect();
    double penWidth = mSymbol->pen().widthF();
    double scaledPenWidth = penWidth / context.sceneScale;
    double halfPenWidth = scaledPenWidth / 2.0;

    double offsetX = mSymbol->getXOffSet() / context.sceneScale;
    double offsetY = mSymbol->getYOffSet() / context.sceneScale;

    rect.translate(offsetX, offsetY);

    mCache.sceneScale = context.sceneScale;
    return mCache.boundingRect = rect.adjusted(
                -halfPenWidth, -halfPenWidth,
                halfPenWidth, halfPenWidth
                );

}

vrsa::graphics::MultiLineFeatureDrawingPolicy::MultiLineFeatureDrawingPolicy(const Symbol *symbol)
    : VectorFeatureDrawingPolicy()
{
    assert(symbol->type() == common::SymbolType::SimpleLineSymbol);
    mSymbol = static_cast<const SimpleLineSymbol*>(symbol);
}


void vrsa::graphics::MultiLineFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom) const
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

    context.painter->save();
    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);

    QPen pen = mSymbol->pen();
    pen.setCapStyle(Qt::SquareCap);
    pen.setWidthF(pen.widthF()/context.sceneScale);
    context.painter->setPen(pen);
    double offsetX = mSymbol->getXOffSet() / context.sceneScale;
    double offsetY = mSymbol->getYOffSet() / context.sceneScale;
    context.painter->translate(offsetX, offsetY);
    context.painter->drawPath(mCache.path);
    mCache.sceneScale = context.sceneScale;
    context.painter->restore();



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
        cacheGeometry(context.geom);
    if (mCache.isGeomValid && context.sceneScale == mCache.sceneScale)
        return mCache.boundingRect;

    QRectF rect = mCache.path.boundingRect();
    double penWidth = mSymbol->pen().widthF();
    double scaledPenWidth = penWidth / mCache.sceneScale;
    double halfPenWidth = scaledPenWidth / 2.0;

    double offsetX = mSymbol->getXOffSet() / context.sceneScale;
    double offsetY = mSymbol->getYOffSet() / context.sceneScale;
    rect.translate(offsetX, offsetY);

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







