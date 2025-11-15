#include "vectordataset.h"
#include "gdal/gdalreader.h"
vrsa::vector::VectorDataset::VectorDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset, const std::string source, vecLayers layers)
    : Dataset{std::move(dataset), source},
      mLayers{std::move(layers)}
{

}

std::size_t vrsa::vector::VectorDataset::layersCount()
{
    return mLayers.size();
}

std::unique_ptr<vrsa::vector::VectorLayer>& vrsa::vector::VectorDataset::getLayer(size_t index)
{
    return mLayers[index];
}

std::string vrsa::vector::VectorDataset::getSource()
{
    return mSource;
}

//void vrsa::vector::VectorDataset::InitializeLayers()
//{

//}
