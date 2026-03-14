#include "dataset.h"
#include "GisDefines.h"
vrsa::gdalwrapper::Dataset::Dataset(gdalwrapper::GdalDatasetPtr ds)
    : mDs{std::move(ds)}
{
    if (mDs)
        mSource = mDs->GetDescription();
}

void vrsa::gdalwrapper::Dataset::SetDatasetType(common::DatasetType dsType) noexcept
{
    mType = dsType;
}

vrsa::common::DatasetType vrsa::gdalwrapper::Dataset::GetDatasetType() const noexcept
{
    return mType;
}


