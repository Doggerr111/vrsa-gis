#include "featuregraphicsitem.h"

vrsa::graphics::FeatureGraphicsItem::FeatureGraphicsItem(DrawingPolicy dPolicy,const Feature& feature)
    : QGraphicsItem(),
      mPolicy{std::move(dPolicy)},
      mFeature{feature}
{

}

QRectF vrsa::graphics::FeatureGraphicsItem::boundingRect() const
{
    DrawingContext context{nullptr, nullptr, nullptr, mFeature->getOGRGeometry(), mWidgetScale};
    return mPolicy->boundingRect(context);
}

void vrsa::graphics::FeatureGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    DrawingContext context{painter, option, widget, mFeature->getOGRGeometry(), mWidgetScale};
    mPolicy->paint(context);
}

void vrsa::graphics::FeatureGraphicsItem::setVisible(bool flagVisible)
{
    QGraphicsItem::setVisible(flagVisible);
}
