#include "featuregraphicsitem.h"
#include "vector/vectorfeature.h"
#include "graphics/drawingcontext.h"
#include <QGraphicsScene>
#include "common/applicationsettings.h"
vrsa::graphics::FeatureGraphicsItem::FeatureGraphicsItem(Feature *feature)
    : QGraphicsItem(),
      mFeature{feature},
      mIsSelected{false},
      mIsLodEnabled{false}
{
    if (mFeature)
    {
        auto& settings = common::ApplicationSettings::instance();
        mRenderer = std::make_unique<FeatureGraphicsItemRenderer>(feature->getStyle(), feature->getGeometryType());
        qDebug()<<feature->getStyle()->getSymbol()->type();
        QObject::connect(feature, &vector::VectorFeature::visibilityChanged, this, &FeatureGraphicsItem::setVisible);
        QObject::connect(feature, &vector::VectorFeature::styleChanged, this, &FeatureGraphicsItem::onFeatureStyleChanged);
        QObject::connect(feature, &vector::VectorFeature::geometryChanged, this,
                         &FeatureGraphicsItem::onVectorFeatureGeometryChanged);
        QObject::connect(feature, &vector::VectorFeature::selectionChanged, this,
                         &FeatureGraphicsItem::onFeatureSelected);
        QObject::connect(feature, &vector::VectorFeature::featureAboutToBeRemoved, this,
                         &FeatureGraphicsItem::onFeatureAboutToBeRemoved);
        QObject::connect(feature, &vector::VectorFeature::ZValueChanged, this, &FeatureGraphicsItem::onZValueChanged);
        QObject::connect(feature, &vector::VectorFeature::symbolUpdated, this, &FeatureGraphicsItem::onSymbolUpdated);
        QObject::connect(&settings, &common::ApplicationSettings::lodSettingsChanged,
                         this, &FeatureGraphicsItem::onLodSettingsChanged);


    }
}

QRectF vrsa::graphics::FeatureGraphicsItem::boundingRect() const
{
    if (!mFeature)
        return QRectF();
    bool isLodNeeded;
    if (mIsLodEnabled && !mFeature->isGeographical())
        isLodNeeded = true;
    else
        isLodNeeded = false;
    DrawingContext context{nullptr, nullptr, nullptr, mFeature->getOGRGeometry(), mWidgetScale, mMapScale, isLodNeeded};
    //qDebug()<< mFeature->getOGRGeometry();
    return mRenderer->boundingRect(context);
}

void vrsa::graphics::FeatureGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!mFeature)
        return;
    bool isLodNeeded;
    if (mIsLodEnabled && !mFeature->isGeographical())
        isLodNeeded = true;
    else
        isLodNeeded = false;
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

void vrsa::graphics::FeatureGraphicsItem::onFeatureSelected(bool selected)
{
    setSelected(selected);
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

void vrsa::graphics::FeatureGraphicsItem::onLodSettingsChanged(bool enabled, common::LodAlgorithmType type)
{
    mIsLodEnabled = enabled;
    mLodAlgo = type;
    mRenderer->update();
}

void vrsa::graphics::FeatureGraphicsItem::onFeatureAboutToBeRemoved()
{
    emit requestRemoval(this);
}
