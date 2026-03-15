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

    qDebug()<< context.option->levelOfDetailFromTransform(context.painter->worldTransform());
    qDebug()<< context.sceneScale;;

    context.painter->save();
    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);

    QPen pen = mSymbol->pen();
    //qDebug()<<pen.widthF()/context.sceneScale;
    //double penWidth = pen.widthF()/context.sceneScale;

    pen.setWidthF(pen.widthF()/context.sceneScale);
    //qDebug()<<pen.widthF();
    context.painter->setPen(pen);
    double offsetX = mSymbol->getXOffSet() / context.sceneScale;
    double offsetY = mSymbol->getYOffSet() / context.sceneScale;
    context.painter->translate(offsetX, offsetY);
    context.painter->drawPath(mCache.path);
    //qDebug() << "PATH" << mCache.path;
    mCache.sceneScale = context.sceneScale;

    context.painter->restore();

}

vrsa::common::GeometryType vrsa::graphics::LineFeatureDrawingPolicy::getType() const
{
    return vrsa::common::GeometryType::LineString;
}


QRectF vrsa::graphics::LineFeatureDrawingPolicy::boundingRect(const DrawingContext &context) const
{

    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);
    if (mCache.isGeomValid && mCache.isBoundingRectValid && (context.sceneScale == mCache.sceneScale))
        return mCache.boundingRect;

    QRectF rect = mCache.path.boundingRect();
    double penWidth = mSymbol->pen().widthF(); //* 3;
    double scaledPenWidth = penWidth / context.sceneScale + 1;
    double halfPenWidth = scaledPenWidth / 2.0;

    double offsetX = mSymbol->getXOffSet() / context.sceneScale;
    double offsetY = mSymbol->getYOffSet() / context.sceneScale;

    rect.translate(offsetX, offsetY);

    mCache.sceneScale = context.sceneScale;
    mCache.isBoundingRectValid = true;
    return mCache.boundingRect = rect.adjusted(
                -halfPenWidth, -halfPenWidth,
                halfPenWidth, halfPenWidth
                );
//    static constexpr QRectF rect = QRectF(-1000,1000,100000,-10000);
//    return rect;

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

//    qDebug()<< context.option->levelOfDetailFromTransform(context.painter->worldTransform());
//    qDebug()<< context.sceneScale;;
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











