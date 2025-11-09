#include "pointfeaturedrawingpolicy.h"

vrsa::graphics::PointFeatureDrawingPolicy::PointFeatureDrawingPolicy(VectorFeatureStyle &style)
    :VectorFeatureDrawingPolicy(style)
{

}

void vrsa::graphics::PointFeatureDrawingPolicy::paint(const DrawingContext &context) const
{
    //context.geom;
    QPen pen = mStyle.getPen();
    //pen.setWidthF(pen.widthF()/mSceneScale);
    context.painter->setPen(pen);
    QBrush brush = mStyle.getBrush();
    brush.setTransform(QTransform(context.painter->worldTransform().inverted())); //обязательно для корректного применения стилей кисти
    context.painter->setBrush(brush);
    //mPointSize=LIPVectorStyle::MMToPixel(mStyle->getPointSize());;
    OGRPoint* point = static_cast<OGRPoint*>(context.geom);
    double x = point->getX();
    double y = point->getY();
    double z = point->getZ();  // если есть Z-координата
    context.painter->drawEllipse(QPointF(x, y), (3/2)/1, (3/2)/1);

}

vrsa::common::GeometryType vrsa::graphics::PointFeatureDrawingPolicy::getType() const
{
    return vrsa::common::GeometryType::Point;
}


QRectF vrsa::graphics::PointFeatureDrawingPolicy::boundingRect() const
{
    return QRectF(0,0,360,180);
}
