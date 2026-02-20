#include "polygonfeaturedrawingpolicy.h"

vrsa::graphics::PolygonFeatureDrawingPolicy::PolygonFeatureDrawingPolicy(Symbol *symbol)
    : VectorFeatureDrawingPolicy()
{
    assert(symbol->type() == common::SymbolType::SimplePolygonSymbol);
    mSymbol = static_cast<const SimplePolygonSymbol*>(symbol);
}


void vrsa::graphics::PolygonFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom) const
{

    mCache.path = QPainterPath();
    OGRPolygon* polygon = (OGRPolygon*)geom;

    // Внешнее кольцо
    OGRLinearRing* exterior = polygon->getExteriorRing();
    if (exterior && exterior->getNumPoints() > 0) {
        mCache.path.moveTo(exterior->getX(0), exterior->getY(0));
        for (int i = 1; i < exterior->getNumPoints(); ++i) {
            mCache.path.lineTo(exterior->getX(i), exterior->getY(i));
        }
        mCache.path.closeSubpath();
    }

    // Внутренние кольца
    for (int i = 0; i < polygon->getNumInteriorRings(); ++i) {
        OGRLinearRing* interior = polygon->getInteriorRing(i);
        if (interior && interior->getNumPoints() > 0) {
            mCache.path.moveTo(interior->getX(0), interior->getY(0));
            for (int j = 1; j < interior->getNumPoints(); ++j) {
                mCache.path.lineTo(interior->getX(j), interior->getY(j));
            }
            mCache.path.closeSubpath(); // Замыкаем
        }
    }
    mCache.isGeomValid=true;

}

void vrsa::graphics::PolygonFeatureDrawingPolicy::paint(const DrawingContext &context)
{
    context.painter->save();
    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);

    QPen pen = mSymbol->pen();
    pen.setWidthF(pen.widthF()/context.sceneScale);
    context.painter->setPen(pen);
    QBrush brush = mSymbol->brush();
    brush.setTransform(QTransform(context.painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
    context.painter->setBrush(brush);

    double offsetX = mSymbol->getXOffSet() / context.sceneScale;
    double offsetY = mSymbol->getYOffSet() / context.sceneScale;
    context.painter->translate(offsetX, offsetY);

    context.painter->drawPath(mCache.path);
    mCache.sceneScale = context.sceneScale;
    context.painter->restore();



}

vrsa::common::GeometryType vrsa::graphics::PolygonFeatureDrawingPolicy::getType() const
{
    return common::GeometryType::Polygon;
}

QRectF vrsa::graphics::PolygonFeatureDrawingPolicy::boundingRect(const DrawingContext &context) const
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

void vrsa::graphics::PolygonFeatureDrawingPolicy::rebuildCache(const DrawingContext &context)
{
}



vrsa::graphics::MultiPolygonFeatureDrawingPolicy::MultiPolygonFeatureDrawingPolicy(Symbol* symbol)
    : VectorFeatureDrawingPolicy()
{
    assert(symbol->type() == common::SymbolType::SimplePolygonSymbol);
    mSymbol = static_cast<const SimplePolygonSymbol*>(symbol);
}

void vrsa::graphics::MultiPolygonFeatureDrawingPolicy::paint(const DrawingContext &context)
{
    context.painter->save();
    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);

    QPen pen = mSymbol->pen();
    pen.setWidthF(pen.widthF()/context.sceneScale);
    context.painter->setPen(pen);
    QBrush brush = mSymbol->brush();
    //brush.setStyle(Qt::Dense5Pattern);
    brush.setTransform(QTransform(context.painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
    context.painter->setBrush(brush);

    double offsetX = mSymbol->getXOffSet() / context.sceneScale;
    double offsetY = mSymbol->getYOffSet() / context.sceneScale;
    context.painter->translate(offsetX, offsetY);

    context.painter->drawPath(mCache.path);
    mCache.sceneScale = context.sceneScale;
    context.painter->restore();
}

vrsa::common::GeometryType vrsa::graphics::MultiPolygonFeatureDrawingPolicy::getType() const
{
    return common::GeometryType::MultiPolygon;
}

QRectF vrsa::graphics::MultiPolygonFeatureDrawingPolicy::boundingRect(const DrawingContext &context) const
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

void vrsa::graphics::MultiPolygonFeatureDrawingPolicy::rebuildCache(const DrawingContext &context)
{

}

void vrsa::graphics::MultiPolygonFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom) const
{
    OGRMultiPolygon* multiPoly = (OGRMultiPolygon*)geom;

    // Собираем все точки
    for (int i = 0; i < multiPoly->getNumGeometries(); ++i)
    {
        OGRPolygon* polygon = (OGRPolygon*)multiPoly->getGeometryRef(i);

        OGRLinearRing* exterior = polygon->getExteriorRing();
        if (exterior && exterior->getNumPoints() > 0) {
            mCache.path.moveTo(exterior->getX(0), exterior->getY(0));
            for (int i = 1; i < exterior->getNumPoints(); ++i) {
                mCache.path.lineTo(exterior->getX(i), exterior->getY(i));
            }
            mCache.path.closeSubpath();
        }

        // Внутренние кольца
        for (int i = 0; i < polygon->getNumInteriorRings(); ++i) {
            OGRLinearRing* interior = polygon->getInteriorRing(i);
            if (interior && interior->getNumPoints() > 0) {
                mCache.path.moveTo(interior->getX(0), interior->getY(0));
                for (int j = 1; j < interior->getNumPoints(); ++j) {
                    mCache.path.lineTo(interior->getX(j), interior->getY(j));
                }
                mCache.path.closeSubpath(); // Замыкаем
            }
        }
    }


        mCache.isGeomValid = true;
}
