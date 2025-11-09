#include "projectmanager.h"

vrsa::services::ProjectManager::ProjectManager(QObject *parent)
    : QObject{parent}
{

}

void vrsa::services::ProjectManager::AddDataset(DatasetPtr dS)
{
    if (!dS)
        VRSA_DEBUG("ProjectManager", "Failed to add dataset: " + dS->GetSource());
    mDatasets.push_back(std::move(dS));
    emit datasetAdded();
}

std::vector<vrsa::services::ProjectManager::DatasetPtr> &vrsa::services::ProjectManager::getDatasets() noexcept
{
    return mDatasets;
}

vrsa::gdalwrapper::Dataset* vrsa::services::ProjectManager::getDatasetBySource(std::string &src)
{
    auto it = std::find_if(mDatasets.begin(), mDatasets.end(),
                               [&src](const DatasetPtr& dS) {
            return dS->GetSource() == src;
        });

        if (it != mDatasets.end())
            return it->get();
        return nullptr;
        //throw vrsa::common::DataSetNotFoundException(src);
}
