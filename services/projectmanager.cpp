#include "projectmanager.h"

vrsa::services::ProjectManager::ProjectManager(QObject *parent)
    : QObject{parent},
      mActiveVectorLayer{nullptr}
{

}

void vrsa::services::ProjectManager::AddDataset(DatasetPtr dS)
{
    if (!dS)
        VRSA_DEBUG("ProjectManager", "Failed to add dataset: " + dS->getSource());
    mDatasets.push_back(std::move(dS));
    emit datasetAdded();
}

std::vector<vrsa::services::ProjectManager::DatasetPtr> &vrsa::services::ProjectManager::getDatasets() noexcept
{
    return mDatasets;
}

vrsa::gdalwrapper::Dataset* vrsa::services::ProjectManager::getDatasetBySource(const std::string src)
{
    auto it = std::find_if(mDatasets.begin(), mDatasets.end(),
                           [&src](const DatasetPtr& dS) {
        return dS->getSource() == src;
    });

    if (it != mDatasets.end())
        return it->get();

    return nullptr;
        //throw vrsa::common::DataSetNotFoundException(src);
}

std::vector<std::string> vrsa::services::ProjectManager::getLayerNames(const std::string &src)
{
    auto it = std::find_if(mDatasets.begin(), mDatasets.end(),
                           [&src](const DatasetPtr& dS) {
        return dS->getSource() == src;
    });

    if (it == mDatasets.end())
    {
        return{};
    }

    auto dS = it->get();
    switch (dS->GetDatasetType())
    {
    case common::DatasetType::Vector:
    {
        auto vDs = static_cast<vector::VectorDataset*>(dS);
        std::vector<std::string> names;
        for (auto& layer: vDs->getLayers())
        {
            names.push_back(layer->getName());
        }
        return names;
    }
    default:
        return{};

    }
}

vrsa::vector::VectorLayer* vrsa::services::ProjectManager::getLayer(const std::string src, int idx)
{
    auto dS = getDatasetBySource(src);
    if (dS == nullptr)
    {
        VRSA_ERROR("ProjectManager", "Can't find dataset:" + src);
        return nullptr;
    }
    if (dS->GetDatasetType() != common::DatasetType::Vector)
    {
        VRSA_ERROR("ProjectManager", "Can't find vector dataset:" + src);
        return nullptr;
    }
    auto vDs = static_cast<vector::VectorDataset*>(getDatasetBySource(src));
    if (vDs)
    {
        try
        {
            return vDs->getLayer(idx).get();
        }
        catch (const std::out_of_range& e)
        {
            VRSA_ERROR("ProjectManager", "Can't find vector layer, passed index out of range:" + std::to_string(idx) + " Dataset:" + src);
        }
    }
    return nullptr;

}

void vrsa::services::ProjectManager::setActiveVectorLayer(const std::string &src, int idx)
{
    auto layer = getLayer(src, idx);
    if (layer)
    {
        mActiveVectorLayer=layer;
        return;
    }
    VRSA_ERROR("ProjectManager", "Cant set active layer with given path:" + src +" and index:" + std::to_string(idx));
}
