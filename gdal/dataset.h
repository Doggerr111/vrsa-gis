#ifndef DATASET_H
#define DATASET_H
#include "GisDefines.h"
#include "gdal/gdalresourcehandles.h"
namespace vrsa {
namespace gdalwrapper {


class Dataset
{
public:
    Dataset(vrsa::gdalwrapper::GdalDatasetPtr mDs, std::string mSource);

    void SetDatasetType(vrsa::common::DatasetType dsType) noexcept;
    vrsa::common::DatasetType GetDatasetType() const noexcept;
    std::string GetSource() const noexcept;

protected:
    vrsa::common::DatasetType mType;
    vrsa::gdalwrapper::GdalDatasetPtr mDs;
    std::string mSource;
};
}
}
#endif // DATASET_H
