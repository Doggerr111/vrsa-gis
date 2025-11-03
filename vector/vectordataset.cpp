#include "vectordataset.h"
#include "gdal/gdalvectorreader.h"
vrsa::vector::VectorDataset::VectorDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset, const std::string source, vecLayers layers)
    : mDs{std::move(dataset)},
      mLayers{std::move(layers)},
      mSource{source}
{

}

std::size_t vrsa::vector::VectorDataset::layersCount()
{
    return mLayers.size();
}

std::string vrsa::vector::VectorDataset::getSource()
{
    return mSource;
}

//void vrsa::vector::VectorDataset::InitializeLayers()
//{

//}
