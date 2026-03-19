#include "webrastergraphicsitem.h"
#include "raster/webrasterdataset.h"

vrsa::graphics::WebRasterGraphicsItem::WebRasterGraphicsItem(raster::WebRasterDataset *webDs)
    : RasterGraphicsItem(webDs),
      mDataset{webDs}
{

}
