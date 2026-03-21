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
    ~WebRasterGraphicsItem();
    // RasterGraphicsItem interface
protected:
    void notifyExtentChanged(const QRectF &mapExtent, const QRect &widgetRect) override;

private:
    raster::WebRasterDataset* mDataset;
    std::atomic<bool> mCancelled{false};


};
}
}
#endif // WEBRASTERGRAPHICSITEM_H
