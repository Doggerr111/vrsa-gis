#include "dataset.h"
#include "GisDefines.h"
vrsa::gdalwrapper::Dataset::Dataset(gdalwrapper::GdalDatasetPtr ds)
    : mDs{std::move(ds)},
      mSourceType{common::DatasetSource::File},
      mUserDatasetName{"Unknown"}
{
    if (mDs)
        mSource = mDs->GetDescription();
}

vrsa::gdalwrapper::Dataset::~Dataset() = default;
void vrsa::gdalwrapper::Dataset::SetDatasetType(common::DatasetType dsType) noexcept
{
    mType = dsType;
}

vrsa::common::DatasetType vrsa::gdalwrapper::Dataset::GetDatasetType() const noexcept
{
    return mType;
}

void vrsa::gdalwrapper::Dataset::setDatasetSourceType(common::DatasetSource type)
{
    mSourceType = type;
}


