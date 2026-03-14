#include "vectordataset.h"

vrsa::vector::VectorDataset::VectorDataset(vrsa::gdalwrapper::GdalDatasetPtr dataset, vecLayers layers)
    : Dataset{std::move(dataset)},
      mLayers{std::move(layers)}
{

}



