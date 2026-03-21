#include "applicationsettings.h"


void vrsa::common::ApplicationSettings::setLodEnabled(bool enabled)
{
    mIsLodEnabled = enabled;
    emit lodSettingsChanged(mIsLodEnabled, mLodAlgorithm);
}

void vrsa::common::ApplicationSettings::setLodAlgorithm(common::LodAlgorithmType algo) noexcept
{
    mLodAlgorithm = algo;
    emit lodSettingsChanged(mIsLodEnabled, mLodAlgorithm);
}

vrsa::common::ApplicationSettings::ApplicationSettings() = default;
