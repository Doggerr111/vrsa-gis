#include "polygonfeaturedrawingpolicy.h"

vrsa::graphics::PolygonFeatureDrawingPolicy::PolygonFeatureDrawingPolicy(VectorFeatureStyle &style)
    : VectorFeatureDrawingPolicy(style)
{

}


void vrsa::graphics::PolygonFeatureDrawingPolicy::cacheGeometry(OGRGeometry *geom)
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

    if (!mCache.isGeomValid)
        cacheGeometry(context.geom);

    QPen pen = mStyle.getPen();
    pen.setCapStyle(Qt::SquareCap);
    pen.setWidthF(calculations::UnitConversion::mmToPixels(pen.widthF())/context.sceneScale);
    context.painter->setPen(pen);
    QBrush brush = mStyle.getBrush();
    //brush.setStyle(Qt::Dense5Pattern);
    brush.setTransform(QTransform(context.painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
    context.painter->setBrush(brush);

    context.painter->drawPath(mCache.path);
    mCache.sceneScale = context.sceneScale;



}

vrsa::common::GeometryType vrsa::graphics::PolygonFeatureDrawingPolicy::getType() const
{
    return common::GeometryType::Polygon;
}

QRectF vrsa::graphics::PolygonFeatureDrawingPolicy::boundingRect(const DrawingContext &context) const
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

void vrsa::graphics::PolygonFeatureDrawingPolicy::rebuildCache(const DrawingContext &context)
{
}


