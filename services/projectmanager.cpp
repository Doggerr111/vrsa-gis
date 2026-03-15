#include "projectmanager.h"
#include "vector/vectorlayer.h"
#include "gdal/gdalreader.h"
#include "featuregraphicsitemfactory.h"
vrsa::services::ProjectManager::ProjectManager(QObject *parent)
    : QObject{parent},
      mActiveVectorLayer{nullptr}
{

}

void vrsa::services::ProjectManager::AddDataset(DatasetPtr dS)
{
    if (!dS)
    {
        VRSA_DEBUG("ProjectManager", "Failed to add new dataset");
        return;
    }
    graphics::FeatureGraphicsItemFactory &itemFactoryRef = graphics::FeatureGraphicsItemFactory::instance();
    switch (dS->GetDatasetType())
    {
    case common::DatasetType::Vector:
    {
        auto vectorDataset = static_cast<vrsa::vector::VectorDataset*>(dS.get());
        for (int i = 0; i < vectorDataset->layersCount(); ++i )
        {
            auto vectorLayer = vectorDataset->getLayer(i);
            if (!vectorLayer)
            {
                VRSA_DEBUG("ProjectManager", "Nullptr layer after VectorDataset::getLayer(size_t indx)");
                continue;
            }
            for (auto vectorFeature: vectorLayer->getFeatures())
            {
                if (!vectorFeature)
                {
                    VRSA_DEBUG("ProjectManager", "Nullptr feature in Vector Layer");
                    continue;
                }
                itemFactoryRef.createForFeature(vectorFeature);
//                auto featItem = std::make_unique<graphics::FeatureGraphicsItem>(vectorFeature);
//                emit featureGraphicsItemCreated(featItem.release()); //передаем владение (сцене)
            }

        }
        break;
    }
    default:break;
    }

    auto* dsPtr = dS.get();
    mDatasets.push_back(std::move(dS));
    emit datasetAdded(dsPtr);
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

vrsa::vector::VectorLayer* vrsa::services::ProjectManager::getLayer(const std::string &src, int idx)
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
        return vDs->getLayer(idx);
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

vrsa::vector::VectorLayer *vrsa::services::ProjectManager::getLayerAssociatedWithFeature(const vector::VectorFeature *feature) const
{

    if (!feature) return nullptr;
    if (!feature->getOGRLayer()) return nullptr;

    for (const auto& dS: mDatasets)
    {
        auto dataset = dS.get();
        if (dataset->GetDatasetType() == common::DatasetType::Vector)
        {
            auto vDs = static_cast<vector::VectorDataset*>(dataset);
            for (const auto& layer: vDs->getLayers())
            {
                if (layer->getOGRLayer() == feature->getOGRLayer())
                    return layer.get();
            }
        }
    }
    return nullptr;

}

vrsa::gdalwrapper::Dataset *vrsa::services::ProjectManager::readDataset(const std::string &source)
{
    vrsa::gdalwrapper::GDALReader reader;
    std::unique_ptr<vrsa::gdalwrapper::Dataset> datasetUPtr;
    try
    {
        datasetUPtr=reader.readDataset(source);
    }
    catch (const vrsa::common::DataSetOpenException& e)
    {
        VRSA_DEBUG("ProjectManager", e.what());
    }
    if (!datasetUPtr)
    {
        VRSA_ERROR("ProjectManager", "Can't read dataset: " + source);
        return nullptr;
    }
    auto rawDs = datasetUPtr.get();
    AddDataset(std::move(datasetUPtr));
    return rawDs;
}

void vrsa::services::ProjectManager::onVectorLayerReadingRequested(const std::string &src)
{
    readDataset(src);

}

