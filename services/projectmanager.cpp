#include "projectmanager.h"
#include "vector/vectorlayer.h"
#include "gdal/gdalreader.h"
#include "featuregraphicsitemfactory.h"
#include "rastergraphicsitemfactory.h"
#include "raster/webrasterdataset.h"
#include "raster/rasterdataset.h"
vrsa::services::ProjectManager::ProjectManager(QObject *parent)
    : QObject{parent},
      mActiveVectorLayer{nullptr}
{

}

vrsa::services::ProjectManager::~ProjectManager()
{
    for (auto& dataset: mDatasets)
        emit datasetAboutToBeRemoved(dataset.get());
}


void vrsa::services::ProjectManager::AddDataset(DatasetPtr dS)
{
    if (!dS)
    {
        VRSA_DEBUG("CORE", "Failed to add new dataset");
        return;
    }
    switch (dS->GetDatasetType())
    {
    case common::DatasetType::Vector:
    {
        //graphics::FeatureGraphicsItemFactory &itemFactoryRef = graphics::FeatureGraphicsItemFactory::instance();
        auto vectorDataset = static_cast<vrsa::vector::VectorDataset*>(dS.get());
        for (int i = 0; i < vectorDataset->layersCount(); ++i )
        {
            auto vectorLayer = vectorDataset->getLayer(i);
            if (!vectorLayer)
            {
                VRSA_DEBUG("CORE", "Nullptr layer after VectorDataset::getLayer(size_t indx)");
                continue;
            }
            for (auto vectorFeature: vectorLayer->getFeatures())
            {
                if (!vectorFeature)
                {
                    VRSA_DEBUG("CORE", "Nullptr feature in Vector Layer");
                    continue;
                }
                graphics::FeatureGraphicsItemFactory::instance().createForFeature(vectorFeature);
//                auto featItem = std::make_unique<graphics::FeatureGraphicsItem>(vectorFeature);
//                emit featureGraphicsItemCreated(featItem.release()); //передаем владение (сцене)
            }

        }
        break;
    }
    case common::DatasetType::Raster:
    {
        if (dS->getDatasetSourceType() != common::DatasetSource::File)
        {
            graphics::RasterGraphicsItemFactory::instance().createForWebDataset
                    (static_cast<raster::WebRasterDataset*>(dS.get()));
        }
        else
        {
            graphics::RasterGraphicsItemFactory::instance().createForFileDataset
                    (static_cast<raster::RasterDataset*>(dS.get()));
        }
    }
    default:
        break;
    }

    auto* dsPtr = dS.get();
    mDatasets.push_back(std::move(dS));
    emit datasetAdded(dsPtr);
}

void vrsa::services::ProjectManager::removeDataset(gdalwrapper::Dataset *dS)
{
    for (auto it = mDatasets.begin(); it != mDatasets.end();)
    {
        if (it->get() == dS)
        {
            emit datasetAboutToBeRemoved(it->get());
            it = mDatasets.erase(it);
        }
        else
            it++;
    }
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
        VRSA_ERROR("CORE", "Can't find dataset:" + src);
        return nullptr;
    }
    if (dS->GetDatasetType() != common::DatasetType::Vector)
    {
        VRSA_ERROR("CORE", "Can't find vector dataset:" + src);
        return nullptr;
    }
    auto vDs = static_cast<vector::VectorDataset*>(getDatasetBySource(src));
    if (vDs)
    {
        return vDs->getLayer(idx);
    }
    return nullptr;

}

std::vector<vrsa::vector::VectorLayer *> vrsa::services::ProjectManager::getProjectedVectorLayers()
{
    std::vector<vector::VectorLayer*> vec;
    for (const auto& dataset: mDatasets)
    {
        if (dataset->GetDatasetType() == common::DatasetType::Vector)
        {

            auto vDs = static_cast<vector::VectorDataset*>(dataset.get());
            for (const auto& layer: vDs->getLayers())
            {
                auto ogr = layer->getOGRLayer();
                if (!ogr) continue;
                auto spatiaRef = ogr->GetSpatialRef();
                if (!spatiaRef) continue;
                if (!spatiaRef->IsGeographic())
                    vec.emplace_back(layer.get());
            }
        }
    }
    return vec;
}

std::vector<std::string> vrsa::services::ProjectManager::getVectorLayersNames() const
{
    std::vector<std::string> vec;
    for (const auto& dataset: mDatasets)
    {
        if (dataset->GetDatasetType() == common::DatasetType::Vector)
        {
            auto vDs = static_cast<vector::VectorDataset*>(dataset.get());
            for (const auto& layer: vDs->getLayers())
            {
                if (!layer) continue;
                vec.emplace_back(layer->getNameAsString());
            }
        }
    }
    return vec;
}

vrsa::vector::VectorLayer *vrsa::services::ProjectManager::getVectorLayerByName(const std::string &name)
{
    for (const auto& dataset: mDatasets)
    {
        switch (dataset->GetDatasetType())
        {
        case common::DatasetType::Vector:
        {
            auto vDs = static_cast<vector::VectorDataset*>(dataset.get());
            auto& layers = vDs->getLayers();
            for (const auto& layer: layers)
            {
                if (layer->getName() == name)
                    return layer.get();
            }
            continue;
        }
        default:
            continue;
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
    VRSA_ERROR("CORE", "Cant set active layer with given path:" + src +" and index:" + std::to_string(idx));
}

vrsa::vector::VectorDataset *vrsa::services::ProjectManager::getDatasetAssociatedWithVectorLayer
                                                        (const vector::VectorLayer *layer) const
{
    if (!layer) return nullptr;
    for (const auto& dataset: mDatasets)
    {
        if (dataset->GetDatasetType() == common::DatasetType::Vector)
        {
            auto vectorDs = static_cast<vector::VectorDataset*>(dataset.get());
            const auto& layers = vectorDs->getLayers();
            auto it = std::find_if(layers.begin(), layers.end(), [layer](const auto& ptr) {
                    return ptr.get() == layer;});
            if (it != layers.end())
                return vectorDs;
        }
    }
    return nullptr;
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

int vrsa::services::ProjectManager::getLayerID(const vector::VectorLayer *layer) const
{
    if (!layer) return -1;
    auto ds = getDatasetAssociatedWithVectorLayer(layer);
    const auto& dsLayers = ds->getLayers();
    for (int i = 0; i < dsLayers.size(); ++i)
    {
        if (dsLayers.at(i).get() == layer)
            return i;
    }
    return -1;
}

std::vector<std::string> vrsa::services::ProjectManager::getRasterDatasetSources() const
{
    std::vector<std::string> rasterSources;
    for (const auto& dS: mDatasets)
    {
        auto dataset = dS.get();
        if (dataset->GetDatasetType() == common::DatasetType::Raster)
        {
            rasterSources.emplace_back(dataset->getSource());
        }
    }
    return rasterSources;
}

std::vector<std::string> vrsa::services::ProjectManager::getVectorDatasetSources() const
{
    std::vector<std::string> vectorSources;
    for (const auto& dS: mDatasets)
    {
        auto dataset = dS.get();
        if (dataset->GetDatasetType() == common::DatasetType::Vector)
        {
            vectorSources.emplace_back(dataset->getSource());
        }
    }
    return vectorSources;
}



vrsa::gdalwrapper::Dataset *vrsa::services::ProjectManager::readDataset(const std::string &source,
                                                                        unsigned int flags)
{
    vrsa::gdalwrapper::GDALReader reader;
    std::unique_ptr<vrsa::gdalwrapper::Dataset> datasetUPtr;
    try
    {
        datasetUPtr=reader.readDataset(source, flags);
    }
    catch (const vrsa::common::DataSetOpenException& e)
    {
        VRSA_ERROR("CORE", e.what());
    }
    if (!datasetUPtr)
    {
        VRSA_ERROR("CORE", "Can't read dataset: " + source);
        return nullptr;
    }
    auto rawDs = datasetUPtr.get();
    AddDataset(std::move(datasetUPtr));
    return rawDs;
}

vrsa::gdalwrapper::Dataset *vrsa::services::ProjectManager::readTMSDataset(const std::string &xml,
                                                                           bool xyz, unsigned int flags)
{
    vrsa::gdalwrapper::GDALReader reader;
    std::unique_ptr<vrsa::gdalwrapper::Dataset> datasetUPtr;
    try
    {
        datasetUPtr=reader.readTMSDataset(xml, flags);
    }
    catch (const vrsa::common::DataSetOpenException& e)
    {
        VRSA_ERROR("CORE", e.what());
    }
    if (!datasetUPtr)
    {
        VRSA_ERROR("CORE", "Can't read TMS dataset: " + xml);
        return nullptr;
    }
    auto rawDs = datasetUPtr.get();
    xyz ? rawDs->setDatasetSourceType(common::DatasetSource::XYZ)
        : rawDs->setDatasetSourceType(common::DatasetSource::TMS);
    AddDataset(std::move(datasetUPtr));
    return rawDs;
}

void vrsa::services::ProjectManager::createPostGISConnection(const QString& connectionName,
                                                             const std::string &connectionString)
{
    if (connectionString.empty()) return;
    auto pgDsUptr = readPostGISDataset(connectionString);
    pgDsUptr ? VRSA_INFO("DATABASE", "Successfuly connected to PostGIS database. Connection name:"
                         + connectionName.toStdString())
             : VRSA_ERROR("DATABASE", "Failed to connect to the PostGIS database. Connection name:"
                         + connectionName.toStdString());
    pgDsUptr->setName(connectionName.toStdString());
    emit postGisDatasetReady(pgDsUptr.get());
    mDatasets.push_back(std::move(pgDsUptr));
}

void vrsa::services::ProjectManager::createXYZConnection(const QString &connectionName, const std::string &xmlString)
{
    if (xmlString.empty()) return;
    auto XYZds = readTMSDataset(xmlString, true, GDAL_OF_RASTER | GDAL_OF_READONLY);
    XYZds ? VRSA_INFO("MAP SERVICES", "Successfuly connected to XYZ service. Connection name:" + connectionName.toStdString()
                     + " Connection string:" + xmlString)
         : VRSA_ERROR("MAP SERVICES", "Failed to connect to XYZ service. Connection name:" + connectionName.toStdString()
                      + " Connection string:" + xmlString);
    XYZds->setName(connectionName.toStdString());
//    emit postGisDatasetReady(pgDsUptr.get());
//    mDatasets.push_back(std::move(pgDsUptr));
}

void vrsa::services::ProjectManager::addPostGISLayer(vector::VectorLayer *pgLayer)
{
    if (!pgLayer)
    {
        VRSA_ERROR("CORE", "Can't add layer from PG database");
        return;
    }
    for (auto vectorFeature: pgLayer->getFeatures())
    {
        if (!vectorFeature)
        {
            VRSA_DEBUG("CORE", "Nullptr feature in Vector Layer");
            continue;
        }
        graphics::FeatureGraphicsItemFactory::instance().createForFeature(vectorFeature);
    }
    emit vectorLayerAdded(pgLayer);
}

std::unique_ptr<vrsa::gdalwrapper::Dataset> vrsa::services::ProjectManager::readPostGISDataset
                                                         (const std::string &connectionString)
{
    vrsa::gdalwrapper::GDALReader reader;
    std::unique_ptr<vrsa::gdalwrapper::Dataset> datasetUPtr;
    try
    {
        datasetUPtr=reader.readDataset(connectionString, GDAL_OF_VECTOR | GDAL_OF_UPDATE);
    }
    catch (const vrsa::common::DataSetOpenException& e)
    {
        return nullptr;
    }
    datasetUPtr->setDatasetSourceType(common::DatasetSource::PostGIS);
    return datasetUPtr;
}

void vrsa::services::ProjectManager::onVectorLayerReadingRequested(const std::string &src)
{
    readDataset(src);

}

