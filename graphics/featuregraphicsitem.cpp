#include "featuregraphicsitem.h"

vrsa::graphics::FeatureGraphicsItem::FeatureGraphicsItem(DrawingPolicy dPolicy,const Feature& feature)
    : QGraphicsItem(),
      mPolicy{std::move(dPolicy)},
      mFeature{feature}
{

}

QRectF vrsa::graphics::FeatureGraphicsItem::boundingRect() const
{

}

void vrsa::graphics::FeatureGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    DrawingContext context{painter, option, widget, mFeature->getOGRGeometry()};
    mPolicy->paint(context);
}
