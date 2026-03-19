#ifndef FILERASTERGRAPHICSITEM_H
#define FILERASTERGRAPHICSITEM_H

#include "rastergraphicsitem.h"

namespace vrsa{
namespace raster{
class RasterDataset;
}
namespace graphics
{
class FileRasterGraphicsItem : public RasterGraphicsItem
{
    Q_OBJECT
public:
    FileRasterGraphicsItem(raster::RasterDataset* dS);

private:
    raster::RasterDataset* mDataset;
};
}
}
#endif // FILERASTERGRAPHICSITEM_H
