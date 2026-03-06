#include "featuregraphicsitem.h"

vrsa::graphics::FeatureGraphicsItem::FeatureGraphicsItem(Renderer renderer, const Feature *feature)
    : QGraphicsItem(),
      mRenderer{std::move(renderer)},
      mFeature{feature},
      mIsSelected{false}
{

}

QRectF vrsa::graphics::FeatureGraphicsItem::boundingRect() const
{
    DrawingContext context{nullptr, nullptr, nullptr, mFeature->getOGRGeometry(), mWidgetScale};
    if (!mFeature->getOGRFeature())
        qDebug()<<"NULLPTR ogr feature. FEATUREGRAPHICSITEM boundingRect";

    if (!mFeature->getOGRGeometry())
        qDebug()<<"NULLPTR ogr GEOM. FEATUREGRAPHICSITEM boundingRect";

//    static int callCount = 0;
//    qDebug() << "BoundingRect from featuregraphicsitem count - " << callCount++;
    return mRenderer->boundingRect(context);
}

void vrsa::graphics::FeatureGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!mFeature->getOGRFeature())
        qDebug()<<"NULLPTR GEOM. FEATUREGRAPHICSITEM PAINT";
    DrawingContext context{painter, option, widget, mFeature->getOGRGeometry(), mWidgetScale};
    mRenderer->paint(context);


}

void vrsa::graphics::FeatureGraphicsItem::setSelected(const bool selected)
{
    mIsSelected = selected;
    qDebug()<<"FeatureGraphicsItem selected!!" << mIsSelected;
    if (mRenderer)
        mRenderer->setFeatureSelected(mIsSelected);
    update();
}

void vrsa::graphics::FeatureGraphicsItem::setVisible(bool flagVisible)
{
    QGraphicsItem::setVisible(flagVisible);
}

void vrsa::graphics::FeatureGraphicsItem::onZValueChanged(int zValue)
{
    //qDebug()<<zValue;
    //qDebug()<<static_cast<double>(zValue);
    setZValue(static_cast<double>(zValue));
}
