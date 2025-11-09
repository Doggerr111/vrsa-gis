#include "dataset.h"

vrsa::gdalwrapper::Dataset::Dataset(gdalwrapper::GdalDatasetPtr ds, std::string src)
    : mDs{std::move(ds)},
      mSource{src}
{

}

void vrsa::gdalwrapper::Dataset::SetDatasetType(common::DatasetType dsType) noexcept
{
    mType = dsType;
}

vrsa::common::DatasetType vrsa::gdalwrapper::Dataset::GetDatasetType() const noexcept
{
    return mType;
}

std::string vrsa::gdalwrapper::Dataset::GetSource() const noexcept
{
    return mSource;
}
