#include "temporarygraphicsitem.h"
#include "graphics/drawingcontext.h"
#include "gdal/ogrconverter.h"
//TemporaryGraphicsItem::TemporaryGraphicsItem(QObject *parent)
//    : QObject{parent}
//{

//}

vrsa::graphics::TemporaryGraphicsItem::TemporaryGraphicsItem(common::GeometryType geomType, TemporaryItemRole role)
    : mGeomType{geomType},
      mRole{role},
      mRenderer{nullptr}
{
    setupRenderer();
    setZValue(common::MAX_Z_VALUE+1);
}

void vrsa::graphics::TemporaryGraphicsItem::setGeometry(const geometry::Geometry &geom)
{
    prepareGeometryChange();
    mGeom = ogr_utils::OGRConverter::toOGR_uniquePTR(geom);
    mRenderer->update();
    update();
    //qDebug()<< "SETTING GEOM IN TEMP ITEM - " << mGeom.get();
}

void vrsa::graphics::TemporaryGraphicsItem::setupRenderer()
{
    if (!mRenderer)
    {
        //VRSA_DEBUG("TemporaryGraphicsItem", "Setting renderer for temp item");
        mRenderer = std::make_unique<FeatureGraphicsItemRenderer>(generateStyle(), mGeomType);
        //qDebug()<<"feature style:" << mStyle.get();
    }
}

vrsa::graphics::VectorFeatureStyle* vrsa::graphics::TemporaryGraphicsItem::generateStyle()
{

    switch (mRole)
    {
    case TemporaryItemRole::Digitizing:
    {
        mStyle = VectorFeatureStyle::createForDigitizing(mGeomType);
        break;
    }
    case TemporaryItemRole::Selection:
    {
        break;
    }
    case TemporaryItemRole::Measurement:
    {
        break;
    }
    case TemporaryItemRole::Preview:
    {
        break;
    }
    case TemporaryItemRole::RubberBand:
    {
        break;
    }
    }
    return mStyle.get();

}

QRectF vrsa::graphics::TemporaryGraphicsItem::boundingRect() const
{
    if (mGeom)
    {
        DrawingContext context{nullptr, nullptr, nullptr, mGeom.get(), mScale};
        return mRenderer->boundingRect(context);
    }
    return QRectF();
}

void vrsa::graphics::TemporaryGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (mGeom)
    {
        DrawingContext context{painter, option, widget, mGeom.get(), mScale};
        mRenderer->paint(context);
    }
}
