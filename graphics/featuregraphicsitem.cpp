#include "featuregraphicsitem.h"

vrsa::graphics::FeatureGraphicsItem::FeatureGraphicsItem(Feature *feature)
    : QGraphicsItem(),
      mFeature{feature},
      mIsSelected{false}
{
    if (mFeature)
    {
        mRenderer = std::make_unique<FeatureGraphicsItemRenderer>(feature->getStyle(), feature->getGeometryType());
        qDebug()<<feature->getStyle()->getSymbol()->type();
        QObject::connect(feature, &vector::VectorFeature::visibilityChanged, this, &FeatureGraphicsItem::setVisible);
        QObject::connect(feature, &vector::VectorFeature::styleChanged, this, &FeatureGraphicsItem::onFeatureStyleChanged);
        QObject::connect(feature, &vector::VectorFeature::geometryChanged, this,
                         &FeatureGraphicsItem::onVectorFeatureGeometryChanged);
        QObject::connect(feature, &vector::VectorFeature::ZValueChanged, this, &FeatureGraphicsItem::onZValueChanged);
    }
}

QRectF vrsa::graphics::FeatureGraphicsItem::boundingRect() const
{
    if (!mFeature)
        return QRectF();
    if (!mFeature->getOGRGeometry())
        return QRectF();
    DrawingContext context{nullptr, nullptr, nullptr, mFeature->getOGRGeometry(), mWidgetScale};
    return mRenderer->boundingRect(context);
}

void vrsa::graphics::FeatureGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!mFeature)
        return;
    if (!mFeature->getOGRGeometry())
        return;
    DrawingContext context{painter, option, widget, mFeature->getOGRGeometry(), mWidgetScale};
    mRenderer->paint(context);


}

void vrsa::graphics::FeatureGraphicsItem::setSelected(const bool selected)
{
    mIsSelected = selected;
    //qDebug()<<"FeatureGraphicsItem selected!!" << mIsSelected;
    if (mRenderer)
        mRenderer->setFeatureSelected(mIsSelected);
    update();
}

void vrsa::graphics::FeatureGraphicsItem::setVisible(bool flagVisible)
{
    qDebug()<<flagVisible;
    QGraphicsItem::setVisible(flagVisible);
}

void vrsa::graphics::FeatureGraphicsItem::onZValueChanged(int zValue)
{
    //qDebug()<<zValue;
    //qDebug()<<static_cast<double>(zValue);
    setZValue(static_cast<double>(zValue));
}

void vrsa::graphics::FeatureGraphicsItem::onGeometryChanged(const geometry::Geometry &geometry)
{
    bool flag = mFeature->setGeometry(geometry);
    if (flag || scene())
    {
        prepareGeometryChange();
        mRenderer->update();
        update();
    }
}

void vrsa::graphics::FeatureGraphicsItem::onFeatureStyleChanged(VectorFeatureStyle *)
{
    qDebug()<<"vector feature style changed!";

}

void vrsa::graphics::FeatureGraphicsItem::onVectorFeatureGeometryChanged()
{
    qDebug()<<"vector feature geometry changed!";
}
