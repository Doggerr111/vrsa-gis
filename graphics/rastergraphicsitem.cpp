#include "rastergraphicsitem.h"
#include "gdal/dataset.h"
vrsa::graphics::RasterGraphicsItem::RasterGraphicsItem(gdalwrapper::Dataset *dS)
    : mItem{std::make_unique<QGraphicsPixmapItem>()},
      mBaseDs{dS}
{

}

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
