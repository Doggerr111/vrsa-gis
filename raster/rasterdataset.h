#ifndef RASTERDATASET_H
#define RASTERDATASET_H
#include "gdal/dataset.h"
#include "raster/rasterchannel.h"
namespace vrsa
{
namespace raster
{

class RasterDataset: public gdalwrapper::Dataset
{

    using channels = std::vector<std::unique_ptr<RasterChannel>>;
public:
    RasterDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset, const std::string source, channels = channels());

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
private:
    channels mRasterChannels;
    //double mGeoTransform;
    QTransform mGeoTransform;
public:
    RasterDataset();
};
}
}

#endif // RASTERDATASET_H
