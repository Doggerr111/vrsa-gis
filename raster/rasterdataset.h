#ifndef RASTERDATASET_H
#define RASTERDATASET_H
#include "gdal/dataset.h"

namespace vrsa
{
namespace raster
{

class RasterDataset: gdalwrapper::Dataset
{

    using channels = std::vector<std::unique_ptr<vrsa::raster::RasterChannel>>;
public:
    VectorDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset, const std::string source, vecLayers = vecLayers());
    std::size_t layersCount();
    std::unique_ptr<VectorLayer>& getLayer(size_t index);
    inline vecLayers& getLayers()
    {
        return mLayers;
    }
    std::string getSource();
private:
    vecLayers mLayers;
public:
    RasterDataset();
};
}
}

#endif // RASTERDATASET_H
