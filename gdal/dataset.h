#ifndef DATASET_H
#define DATASET_H
#include "gdal/gdalresourcehandles.h"

namespace vrsa {
namespace common{
enum class DatasetType: int;
}
namespace gdalwrapper {


class Dataset
{
public:
    Dataset(vrsa::gdalwrapper::GdalDatasetPtr mDs);

    void SetDatasetType(vrsa::common::DatasetType dsType) noexcept;
    vrsa::common::DatasetType GetDatasetType() const noexcept;
    inline std::string getSource() noexcept { return mSource; }

protected:
    vrsa::common::DatasetType mType;
    vrsa::gdalwrapper::GdalDatasetPtr mDs;
    std::string mSource;
};
}
}
#endif // DATASET_H
