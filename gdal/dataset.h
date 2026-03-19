#ifndef DATASET_H
#define DATASET_H
#include "gdal/gdalresourcehandles.h"

namespace vrsa {
namespace common{
enum class DatasetType: int;
enum class DatasetSource: int;
}
namespace gdalwrapper {


class Dataset
{
public:
    Dataset(vrsa::gdalwrapper::GdalDatasetPtr mDs);

    void SetDatasetType(vrsa::common::DatasetType dsType) noexcept;
    vrsa::common::DatasetType GetDatasetType() const noexcept;
    inline std::string getSource() const noexcept { return mSource; }

    void setDatasetSourceType (vrsa::common::DatasetSource type);
    vrsa::common::DatasetSource getDatasetSourceType() const noexcept { return mSourceType; };

    void setName(const std::string& userDsName) { mUserDatasetName = userDsName; };
    std::string getName() const { return mUserDatasetName; };


protected:
    vrsa::common::DatasetType mType;
    vrsa::common::DatasetSource mSourceType;
    vrsa::gdalwrapper::GdalDatasetPtr mDs;
    std::string mSource;
    std::string mUserDatasetName;
};
}
}
#endif // DATASET_H
