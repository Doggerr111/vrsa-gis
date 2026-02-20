#include "featuregraphicsitem.h"

vrsa::graphics::FeatureGraphicsItem::FeatureGraphicsItem(Renderer renderer, const Feature *feature)
    : QGraphicsItem(),
      mRenderer{std::move(renderer)},
      mFeature{feature}
{

}

QRectF vrsa::graphics::FeatureGraphicsItem::boundingRect() const
{
    DrawingContext context{nullptr, nullptr, nullptr, mFeature->getOGRGeometry(), mWidgetScale};
    return mRenderer->boundingRect(context);
}

void vrsa::graphics::FeatureGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    DrawingContext context{painter, option, widget, mFeature->getOGRGeometry(), mWidgetScale};
    mRenderer->paint(context);

}

void vrsa::graphics::FeatureGraphicsItem::setVisible(bool flagVisible)
{
    QGraphicsItem::setVisible(flagVisible);
}
