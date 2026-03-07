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
    //qDebug() << "Item:" << static_cast<void*>(this);
    setZValue(common::MAX_Z_VALUE+1);
}

void vrsa::graphics::TemporaryGraphicsItem::setGeometry(const geometry::Geometry &geom)
{

    //qDebug() << "Item: setGeometry" << static_cast<void*>(this);
//    qDebug() << "Scene:" << scene();
//    qDebug() << "Parent:" << parentItem();
//    qDebug() << "Flags:" << flags();
//    qDebug() << "Item:" << static_cast<void*>(this);

    if (!scene())
        return;
    prepareGeometryChange();
    mGeom = ogr_utils::OGRConverter::toOGR_uniquePTR(geom);
    mRenderer->update();
    update();
}

void vrsa::graphics::TemporaryGraphicsItem::setupRenderer()
{
    if (!mRenderer)
    {
        mRenderer = std::make_unique<FeatureGraphicsItemRenderer>(generateStyle(), mGeomType);
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
        mStyle = VectorFeatureStyle::createForSelection(mGeomType);
        qDebug()<<"стиль " << mStyle.get();
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
    //qDebug() << "Item: style" << static_cast<void*>(this);
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
