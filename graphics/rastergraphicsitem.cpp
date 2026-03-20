#include "rastergraphicsitem.h"
#include "gdal/dataset.h"
vrsa::graphics::RasterGraphicsItem::RasterGraphicsItem(gdalwrapper::Dataset *dS)
    : mItem{std::make_unique<QGraphicsPixmapItem>()},
      mBaseDs{dS}
{

}

vrsa::graphics::RasterGraphicsItem::~RasterGraphicsItem() = default;

void vrsa::graphics::RasterGraphicsItem::setPixmap(const QPixmap &pixmap)
{
    mItem->setPixmap(pixmap);
}

void vrsa::graphics::RasterGraphicsItem::setTransform(const QTransform &transform)
{
    mItem->setTransform(transform);
}

void vrsa::graphics::RasterGraphicsItem::onZValueChanged(int zValue)
{

}

void vrsa::graphics::RasterGraphicsItem::onVisibleExtentChanged(const QRectF &mapExtent, const QRect &widgetRect)
{
    notifyExtentChanged(mapExtent, widgetRect);
}
