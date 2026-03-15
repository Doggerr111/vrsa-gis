#ifndef RASTERDATASET_H
#define RASTERDATASET_H
#include "gdal/dataset.h"
#include "raster/rasterchannel.h"
namespace vrsa
{
namespace raster
{

class RasterDataset: public QObject, public gdalwrapper::Dataset
{
    Q_OBJECT
    using channels = std::vector<std::unique_ptr<RasterChannel>>;
public:
    RasterDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset, channels = channels());

    std::unique_ptr<RasterChannel>& getChannel(size_t index);
    inline std::size_t channelsCount() const noexcept
    {
        return mRasterChannels.size();
    }
    inline channels& getChannels() noexcept
    {
        return mRasterChannels;
    }
    QTransform getGeoTransform();
    QRectF getBoundingBox();
    inline void setZValue(int zValue) noexcept
    {
        mZValue = zValue;
    }
signals:
    void ZValueChanged(int);
private:
    channels mRasterChannels;
    //double mGeoTransform;
    QTransform mGeoTransform;
    int mZValue;
public:
    RasterDataset();
};
}
}

#endif // RASTERDATASET_H
