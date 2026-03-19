#ifndef WEBRASTERGRAPHICSITEM_H
#define WEBRASTERGRAPHICSITEM_H
#include "rastergraphicsitem.h"

namespace vrsa
{
namespace raster
{
class WebRasterDataset;
}
namespace graphics
{

class WebRasterGraphicsItem : public RasterGraphicsItem
{
    Q_OBJECT
public:
    WebRasterGraphicsItem(raster::WebRasterDataset* webDs);

private:
    raster::WebRasterDataset* mDataset;
};
}
}
#endif // WEBRASTERGRAPHICSITEM_H
