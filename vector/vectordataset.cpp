#include "vectordataset.h"
#include "gdal/gdalreader.h"
vrsa::vector::VectorDataset::VectorDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset, vecLayers layers)
    : Dataset{std::move(dataset)},
      mLayers{std::move(layers)}
{

}



