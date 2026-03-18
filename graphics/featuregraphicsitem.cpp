#include "featuregraphicsitem.h"
#include "vector/vectorfeature.h"
#include "graphics/drawingcontext.h"
#include <QGraphicsScene>
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
        QObject::connect(feature, &vector::VectorFeature::symbolUpdated, this, &FeatureGraphicsItem::onSymbolUpdated);
    }
}

QRectF vrsa::graphics::FeatureGraphicsItem::boundingRect() const
{
    if (!mFeature)
        return QRectF();
    bool isLodNeeded = !mFeature->isGeographical();
    DrawingContext context{nullptr, nullptr, nullptr, mFeature->getOGRGeometry(), mWidgetScale, mMapScale, isLodNeeded};
    //qDebug()<< mFeature->getOGRGeometry();
    return mRenderer->boundingRect(context);
}

void vrsa::graphics::FeatureGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!mFeature)
        return;
    bool isLodNeeded = !mFeature->isGeographical();
    DrawingContext context{painter, option, widget, mFeature->getOGRGeometry(), mWidgetScale, mMapScale, isLodNeeded};
    mRenderer->paint(context);
}

const OGRGeometry *vrsa::graphics::FeatureGraphicsItem::getFeatureGeometry() const
{
    if (mFeature)
        return mFeature->getOGRGeometry();
    return nullptr;
}

const vrsa::common::GeometryType vrsa::graphics::FeatureGraphicsItem::getFeatureGeometryType() const
{
    if (mFeature)
        return mFeature->getGeometryType();
    return common::GeometryType::Unknown;
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

void vrsa::graphics::FeatureGraphicsItem::onSymbolUpdated()
{

    if (mRenderer)
    {
        prepareGeometryChange();
        mRenderer->updateSymbol();
        update();
    }
}
