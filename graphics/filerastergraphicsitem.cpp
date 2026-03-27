#include "filerastergraphicsitem.h"
#include "raster/rasterdataset.h"

vrsa::graphics::FileRasterGraphicsItem::FileRasterGraphicsItem(raster::RasterDataset *dS)
    : RasterGraphicsItem(dS),
      mDataset{dS}
{
    if (dS)
    {
        mItem->setTransform(dS->getGeoTransform());
        QObject::connect(dS, &raster::RasterDataset::ZValueChanged, this, &RasterGraphicsItem::onZValueChanged);
        QObject::connect(dS, &raster::RasterDataset::visibilityChanged, this, &RasterGraphicsItem::onVisibilityChanged);
    }
}

vrsa::graphics::FileRasterGraphicsItem::~FileRasterGraphicsItem() = default;
