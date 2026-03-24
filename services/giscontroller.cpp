#include "giscontroller.h"
#include "vectordatasetform.h"
#include "attributetableform.h"
#include "vectorstylingform.h"
#include "tools/digitizingtoolfactory.h"
#include "tools/maptoolfactory.h"
#include "vectorlayercreationform.h"
#include "postgisconnectionform.h"
#include "mapserviceconnectionform.h"
#include "rasterproccessingform.h"
#include "geosoperationform.h"
#include "measurementform.h"
#include "vectorreprojectionform.h"
#include "vectorlayertools/spatial_operations/bufferoperation.h"
#include "vectorlayertools/spatial_operations/triangulationoperation.h"
#include "vectorlayertools/spatial_operations/voronoioperation.h"
#include "vectorlayertools/spatial_operations/intersectionoperation.h"
#include "vectorlayertools/spatial_operations/unionoperation.h"
#include "vectorlayertools/spatial_operations/differenceoperation.h"
#include "vectorlayertools/spatial_operations/symdifferenceoperation.h"
#include "rastertools/clipprocessor.h"
#include "rastertools/reprojectprocessor.h"
#include "rastertools/isolinesprocessor.h"

#include "graphics/featuregraphicsitemfactory.h"
#include "graphics/mapscene.h"
#include "common/logger.h"
#include "services/projectmanager.h"
#include "gdal/gdalreader.h"
#include "calculations/mapcalculations.h"
#include "spatialref/spatialrefdatabase.h"
#include "customWidgets/mapholder.h"
#include "customWidgets/treewidget.h"
#include "raster/webrasterdataset.h"
#include "vectorlayertools/vectorlayercreator.h"
#include "graphics/symbols/symbolrenderer.h"
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QButtonGroup>
#include <QLabel>
#include <QGraphicsView>
#include <QStatusBar>
#include <crscombobox.h>
#include "applicationsettings.h"
vrsa::services::GISController::GISController(QObject *parent)
    : QObject{parent},
      mRenderer{std::make_unique<graphics::SymbolRenderer>()},
      mVectorCreator{std::make_unique<vector::VectorLayerCreator>()},
      mProjectManager{std::make_unique<services::ProjectManager>()}
{
    if (!mRenderer || !mVectorCreator || !mProjectManager)
    {
        VRSA_ERROR("CORE", "Error while constructing GIScontroller");
        throw std::runtime_error("Error while constructing GIScontroller");
    }
    connect(this, &GISController::updateLegendIconsRequired, mRenderer.get(),
            &graphics::SymbolRenderer::renderToIcon);
    connect(this, &GISController::vectorLayerCreationRequested, mVectorCreator.get(),
            &vector::VectorLayerCreator::onLayerCreationRequested);
    connect(this, &GISController::datasetReadingRequested, mProjectManager.get(),
            &ProjectManager::onVectorLayerReadingRequested);
    connect(mProjectManager.get(), &ProjectManager::datasetAdded, this,
            &GISController::onDatasetAdded);
    connect(mProjectManager.get(), &ProjectManager::postGisDatasetReady, this,
            &GISController::onPostGisDatasetAdded);
    connect(mProjectManager.get(), &ProjectManager::vectorLayerAdded, this,
            &GISController::onVectorLayerAdded);
    connect(mVectorCreator.get(),  &vector::VectorLayerCreator::vectorLayerReadingRequested, mProjectManager.get(),
            &ProjectManager::onVectorLayerReadingRequested);

}

void vrsa::services::GISController::setupViewAndIntitizlizeScene()
{

    auto view = mComps.mapView;
    mMapScene = new vrsa::graphics::MapScene();
    if (!mMapScene)
        return;
    mMapScene->setMapHolderScale(view->getMapHolderScale());
    view->setScene(mMapScene);

    connect(view, &MapHolder::scaleChanged, mMapScene, &graphics::MapScene::onMapHolderScaleChanged);
    connect(view, &MapHolder::mousePressed, mMapScene, &graphics::MapScene::onMapHolderMousePressed);
    connect(view, &MapHolder::scaleChanged, this, &GISController::onMapHolderScaleChanged);
    connect(view, &MapHolder::extentChanged, mMapScene, &graphics::MapScene::onMapHolderExtentChanged);
    connect(mMapScene, &graphics::MapScene::panningRequested, view, &MapHolder::onPanningRequested);

    connect(this, &vrsa::services::GISController::projectCRSChanged, view, &MapHolder::onCrsChanged);
    //связываем сигналы с фабрик (синглтон), для создания графических объектов и их автоматического добавления на сцену
    connect(&graphics::FeatureGraphicsItemFactory::instance(), &graphics::FeatureGraphicsItemFactory::featureGraphicsItemCreated,
            mMapScene, &graphics::MapScene::onFeatureGraphicsItemCreated);
    connect(&graphics::RasterGraphicsItemFactory::instance(), &graphics::RasterGraphicsItemFactory::rasterGraphicsItemCreated,
            mMapScene, &graphics::MapScene::onRasterGraphicsItemCreated);
    connect(mMapScene, &vrsa::graphics::MapScene::mouseMoved, this, &GISController::onMouseCoordinatesChanged);

    //сигнал для очищения графических объектов при удалении датасета
    connect(mProjectManager.get(), &ProjectManager::datasetAboutToBeRemoved,
            mMapScene, &graphics::MapScene::onDatasetRemoved);
    view->scale(1,-1);
    view->recalculateScale();
}

void vrsa::services::GISController::setup()
{
    setupLegendTreeWidgets();
    setupViewAndIntitizlizeScene();

    connect(mComps.crsCombo, QOverload<int>::of(&CRSComboBox::currentIndexChanged), this,
            &GISController::onCRSComboBoxIndexChanged);
    connect(this, qOverload<const QIcon&>(&GISController::activeLayerChanged), this,
            qOverload<const QIcon&>(&GISController::onActiveLayerChanged));
    connect(this, qOverload<const QString&>(&GISController::activeLayerChanged), this,
            qOverload<const QString&>(&GISController::onActiveLayerChanged));
    connect(mComps.singleSelectionBtn, &QPushButton::clicked, this,
            &GISController::onSingleSelectionToolClicked);
    connect(mComps.rectSeletionBtn, &QPushButton::clicked, this,
            &GISController::onRectSelectionToolClicked);
    connect(mComps.geometryEditBtn, &QPushButton::clicked, this,
            &GISController::onGeometryEditToolClicked);
    connect(mComps.addFeatureBtn, &QPushButton::clicked, this, &GISController::onDigitizingToolClicked);
    connect(mComps.rulerBtn, &QPushButton::clicked, this, &GISController::onRulerToolClicked);


    mComps.lodBtn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mComps.lodBtn, &QToolButton::clicked, this, [](bool enabled){
        common::ApplicationSettings::instance().setLodEnabled(enabled); });
    connect(mComps.lodBtn, &QToolButton::customContextMenuRequested,
            this, &GISController::showContextMenuLOD);

    //==========ПОДЛЮЧЕНИЕ СИГНАЛОВ QAction==========
    //вектор (создание-открытие)
    connect(mComps.actionOpenLayer, &QAction::triggered,
            this, &GISController::onOpenLayerActionTriggered);
    connect(mComps.actionCreateLayer, &QAction::triggered,
            this, &GISController::onCreateLayerActionTriggered);
    connect(mComps.actionCreatePointLayer, &QAction::triggered,
            this, &GISController::onCreatePointLayerActionTriggered);
    connect(mComps.actionCreateLineLayer, &QAction::triggered,
            this, &GISController::onCreateLineLayerActionTriggered);
    connect(mComps.actionCreatePolygonLayer, &QAction::triggered,
            this, &GISController::onCreatePolygonLayerActionTriggered);
    //геопроцессинг
    connect(mComps.actionCreateBuffer, &QAction::triggered,
            this, &GISController::onCreateBufferActionTriggered);
    connect(mComps.actionCreateTriangulation, &QAction::triggered,
            this, &GISController::onCreateTriangulationActionTriggered);
    connect(mComps.actionCreateVoronoiDiag, &QAction::triggered,
            this, &GISController::onCreateVoronoiActionTriggered);
    //overlay
    connect(mComps.actionCreateIntersection, &QAction::triggered,
            this, &GISController::onCreateIntersectionActionTriggered);
    connect(mComps.actionCreateUnion, &QAction::triggered,
            this, &GISController::onCreateUnionActionTriggered);
    connect(mComps.actionCreateDifference, &QAction::triggered,
            this, &GISController::onCreateDifferenceActionTriggered);
    connect(mComps.actionCreateSymDifference, &QAction::triggered,
            this, &GISController::onCreateSymDifferenceActionTriggered);
    //reproject vector
    connect(mComps.actionReprojectVector, &QAction::triggered,
            this, &GISController::onReprojectVectorActionTriggered);
    //rasters
    connect(mComps.actionOpenRasterLayer, &QAction::triggered,
            this, &GISController::onOpenRasterLayerActionTriggered);
    connect(mComps.actionReprojectRaster, &QAction::triggered,
            this, &GISController::onReprojectRasterLayerActionTriggered);
    connect(mComps.actionCutRasterByVectorMask, &QAction::triggered,
            this, &GISController::onCutRasterByVectorMaskActionTriggered);
    connect(mComps.actionCreateIsolines, &QAction::triggered,
            this, &GISController::onCreateIsolinesActionTriggered);


    //сервисы
    connect(mComps.actionWMSConnection, &QAction::triggered,
            this, &GISController::onWMSConnectionTriggered);
    connect(mComps.actionXYZConnection, &QAction::triggered,
            this, &GISController::onXYZConnectionTriggered);

    //бд
    connect(mComps.actionPostGisConnection, &QAction::triggered,
            this, &GISController::onPostGisConnectionActionTriggered);
}

void vrsa::services::GISController::setupLegendTreeWidgets()
{
    auto treeWidget = mComps.mainLegendTree;
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    treeWidget->setItemsExpandable(false);
    treeWidget->expandAll();
    connect(treeWidget, &TreeWidget::customContextMenuRequested,
            this, &services::GISController::showContextMenu);
    connect(treeWidget, &TreeWidget::itemChanged,
            this, &services::GISController::onLayerTreeDataChanged);
    connect(treeWidget, &TreeWidget::itemDoubleClicked,
            this, &services::GISController::onLayerTreeItemDoubleClicked);
    connect(treeWidget, &TreeWidget::itemDragRequested,
            this, &services::GISController::onItemDragRequested);
    connect(treeWidget, &TreeWidget::itemDropped,
            this, &services::GISController::onItemDropped);


    auto treeWidgetPG = mComps.postGisTree;
    treeWidgetPG->setContextMenuPolicy(Qt::CustomContextMenu);
    treeWidgetPG->setItemsExpandable(false);
    treeWidgetPG->expandAll();
    connect(treeWidgetPG, &TreeWidget::customContextMenuRequested,
            this, &services::GISController::showContextMenuPG);
    //    connect(treeWidgetPG, &TreeWidget::itemChanged,
    //            this, &services::GISController::onLayerTreeDataChanged);
    //    connect(treeWidgetPG, &TreeWidget::itemDoubleClicked,
    //            this, &services::GISController::onLayerTreeItemDoubleClicked);
    //    connect(treeWidgetPG, &TreeWidget::itemDragRequested,
    //            this, &services::GISController::onItemDragRequested);
    //    connect(treeWidgetPG, &TreeWidget::itemDropped,
    //            this, &services::GISController::onItemDropped);
}



void vrsa::services::GISController::setupViewComponents(const ViewComponents &comp)
{
    mComps = comp;
    std::string errorMsg;
    if (!mComps.isValid(errorMsg))
    {
        VRSA_ERROR("CORE", "Error while setting up view components:" + errorMsg);
        return;
    }
    VRSA_INFO("CORE", "View components successfully configured");
    setup();
}


void vrsa::services::GISController::onDatasetAdded(gdalwrapper::Dataset *dS)
{
    if (!dS)
        return;
    //заполяем treeWidget (легенду)
    std::string source = dS->getSource();
    auto* dSItem = new QTreeWidgetItem();
    dSItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/dataset.png"));
    dSItem->setText(DATA_COLUMN, QString::fromStdString(source));
    dSItem->setToolTip(DATA_COLUMN, QString::fromStdString(source));
    dSItem->setData(DATA_COLUMN, common::DatasetPathRole, QString::fromStdString(source));
    dSItem->setCheckState(DATA_COLUMN,Qt::Checked);
    dSItem->setFlags(dSItem->flags() | Qt::ItemIsDragEnabled );
    switch (dS->GetDatasetType())
    {
    case common::DatasetType::Vector:
    {
        auto vDs = static_cast<vrsa::vector::VectorDataset*>(dS);
        dSItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_VectorDataset));
        dSItem->setData(DATA_COLUMN, common::DatasetTypeRole, QVariant::fromValue(common::DatasetType::Vector));
        for (int i = 0; i < vDs->layersCount(); ++i)
        {
            auto layer = vDs->getLayer(i);
            if (!layer)
                continue;
            auto* layerItem = new QTreeWidgetItem(dSItem);
            layerItem->setCheckState(DATA_COLUMN,Qt::Checked);
            layerItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_VectorLayer));
            layerItem->setData(DATA_COLUMN, common::DatasetPathRole, QString::fromStdString(source));
            layerItem->setData(DATA_COLUMN, common::LayerIDRole, i);
            layerItem->setData(DATA_COLUMN, common::LayerTypeRole, QVariant::fromValue(layer->getGeomType()));

            const char* layerName = layer->getName();
            const char* displayName = "unknown";
            if (layerName && layerName[0] != '\0')
                displayName = layerName;
            layerItem->setText(DATA_COLUMN, QString::fromUtf8(displayName));

            if (auto layerStyle = layer->getStyle())
            {
                QIcon icon = QIcon();
                emit updateLegendIconsRequired(layerStyle->getSymbol(), icon);
                layerItem->setIcon(DATA_COLUMN, icon);
            }
            else
                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/unknown.png"));
        }
        break;
    }

    case common::DatasetType::Raster:
    {
        switch (dS->getDatasetSourceType())
        {
        case common::DatasetSource::File:
        {
            //TODO ПЕРЕНЕСТИ В PROJECT MANAGER!
            auto rDs = static_cast<vrsa::raster::RasterDataset*>(dS);
            //mMapScene->addRasterDataset(rDs); //ВОТ ЭТО ПЕРЕНЕСТИ И ЧЕРЕЗ ФАБРИКУ ВСЕ 1!!
            dSItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_RasterDataset));
            dSItem->setData(DATA_COLUMN, common::DatasetTypeRole, QVariant::fromValue(common::DatasetType::Raster));
            for (int i=0; i<rDs->channelsCount(); ++i)
            {
                auto& layer = rDs->getChannel(i);
                //mMapScene->addLayer(layer);
                auto* layerItem = new QTreeWidgetItem(dSItem);

                layerItem->setCheckState(DATA_COLUMN,Qt::Checked);

                layerItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_RasterLayer));
                layerItem->setData(DATA_COLUMN, common::DatasetPathRole, QString::fromStdString(source));
                layerItem->setData(DATA_COLUMN, common::LayerIDRole, i);

                QString displayName = tr("Band №");
                layerItem->setText(DATA_COLUMN, displayName + QString::number(i+1));
                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/rasterTreeItem.png"));

            }
            break;
        }
        case common::DatasetSource::XYZ:
        case common::DatasetSource::TMS:
        {
            auto rDs = static_cast<vrsa::raster::WebRasterDataset*>(dS);
            dSItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_RasterDataset));
            dSItem->setData(DATA_COLUMN, common::DatasetTypeRole, QVariant::fromValue(common::DatasetType::Raster));
            break;
        }
        default:
            break;
        } //dsRasterSourceType
        break;
    }

    default:
        VRSA_WARNING("CORE", "Данный тип датасета пока не поддерживается");
    } // Ds type
    mComps.mainLegendTree->addTopLevelItem(dSItem);
    mComps.mainLegendTree->expandAll();
    syncZOrderWithTree();

}

void vrsa::services::GISController::onPostGisDatasetAdded(gdalwrapper::Dataset *dS)
{
    if (!dS) return;
    //заполяем treeWidget (легенду)
    std::string source = dS->getSource();
    auto* dSItem = new QTreeWidgetItem();
    dSItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/postgis.png"));
    dSItem->setText(DATA_COLUMN, QString::fromStdString(dS->getName()));
    dSItem->setToolTip(DATA_COLUMN, QString::fromStdString(source));
    dSItem->setData(DATA_COLUMN, common::DatasetPathRole, QString::fromStdString(source));
    dSItem->setCheckState(DATA_COLUMN,Qt::Checked);
    dSItem->setFlags(dSItem->flags() | Qt::ItemIsDragEnabled );
    switch (dS->GetDatasetType())
    {
    case common::DatasetType::Vector:
    {
        auto vDs = static_cast<vrsa::vector::VectorDataset*>(dS);
        dSItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_VectorDataset));
        dSItem->setData(DATA_COLUMN, common::DatasetTypeRole, QVariant::fromValue(common::DatasetType::Vector));
        for (int i = 0; i < vDs->layersCount(); ++i)
        {
            auto layer = vDs->getLayer(i);
            if (!layer)
                continue;
            auto* layerItem = new QTreeWidgetItem(dSItem);
            layerItem->setCheckState(DATA_COLUMN,Qt::Checked);
            layerItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_VectorLayer));
            layerItem->setData(DATA_COLUMN, common::DatasetPathRole, QString::fromStdString(source));
            layerItem->setData(DATA_COLUMN, common::LayerIDRole, i);
            layerItem->setData(DATA_COLUMN, common::LayerTypeRole, QVariant::fromValue(layer->getGeomType()));

            const char* layerName = layer->getName();
            const char* displayName = "unknown";
            if (layerName && layerName[0] != '\0')
                displayName = layerName;
            layerItem->setText(DATA_COLUMN, QString::fromUtf8(displayName));
            layerItem->setIcon(DATA_COLUMN, getIconForGeometryType(layer->getGeomType()));
        }
        break;
    }
    default:
        VRSA_WARNING("CORE", "Данный тип датасета пока не поддерживается");
    }
    mComps.postGisTree->addTopLevelItem(dSItem);
    mComps.postGisTree->expandAll();
}

void vrsa::services::GISController::onVectorLayerAdded(vector::VectorLayer *layer)
{
    //ищем dataset
    auto dS = mProjectManager->getDatasetAssociatedWithVectorLayer(layer);
    if (!dS)
        return;
    QTreeWidgetItem* dsItem = nullptr;
    for (int i = 0; i < mComps.mainLegendTree->topLevelItemCount(); ++i)
    {
        auto topLevelItem = mComps.mainLegendTree->topLevelItem(i);
        auto itemType = topLevelItem->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>();
        auto& pM = mProjectManager;
        if (itemType == common::ItemType_VectorDataset)
        {
            auto src = topLevelItem->data(DATA_COLUMN, common::DatasetPathRole).toString().toStdString();
            auto dsSrc = dS->getSource();
            if (src == dsSrc)
            {
                dsItem = topLevelItem;
                break;
            }
        }
    }
    if (!dsItem) //если нету итема датасета
    {
        std::string source = dS->getSource();
        dsItem = new QTreeWidgetItem();
        dsItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/dataset.png"));
        dsItem->setText(DATA_COLUMN, QString::fromStdString(source));
        dsItem->setToolTip(DATA_COLUMN, QString::fromStdString(source));
        dsItem->setData(DATA_COLUMN, common::DatasetPathRole, QString::fromStdString(source));
        dsItem->setCheckState(DATA_COLUMN,Qt::Checked);
        dsItem->setFlags(dsItem->flags() | Qt::ItemIsDragEnabled );
        dsItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_VectorDataset));
        dsItem->setData(DATA_COLUMN, common::DatasetTypeRole, QVariant::fromValue(common::DatasetType::Vector));
        mComps.mainLegendTree->addTopLevelItem(dsItem);
    }

    auto* layerItem = new QTreeWidgetItem(dsItem);
    layerItem->setCheckState(DATA_COLUMN,Qt::Checked);
    layerItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_VectorLayer));
    layerItem->setData(DATA_COLUMN, common::DatasetPathRole, QString::fromStdString(dS->getSource()));
    auto id = mProjectManager->getLayerID(layer);
    qDebug()<<id;
    layerItem->setData(DATA_COLUMN, common::LayerIDRole, id);
    layerItem->setData(DATA_COLUMN, common::LayerTypeRole, QVariant::fromValue(layer->getGeomType()));

    const char* layerName = layer->getName();
    const char* displayName = "unknown";
    if (layerName && layerName[0] != '\0')
        displayName = layerName;
    layerItem->setText(DATA_COLUMN, QString::fromUtf8(displayName));

    if (auto layerStyle = layer->getStyle())
    {
        QIcon icon = QIcon();
        emit updateLegendIconsRequired(layerStyle->getSymbol(), icon);
        layerItem->setIcon(DATA_COLUMN, icon);
    }
    else
        layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/unknown.png"));

    mComps.mainLegendTree->expandAll();
    syncZOrderWithTree();
}




//    switch (dS->GetDatasetType())
//    {
//    case common::DatasetType::Vector:
//    {
//        auto vDs = static_cast<vrsa::vector::VectorDataset*>(dS);
//        dSItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_VectorDataset));
//        dSItem->setData(DATA_COLUMN, common::DatasetTypeRole, QVariant::fromValue(common::DatasetType::Vector));
//        for (int i = 0; i < vDs->layersCount(); ++i)
//        {
//            auto layer = vDs->getLayer(i);
//            if (!layer)
//                continue;
//            auto* layerItem = new QTreeWidgetItem(dSItem);
//            layerItem->setCheckState(DATA_COLUMN,Qt::Checked);
//            layerItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_VectorLayer));
//            layerItem->setData(DATA_COLUMN, common::DatasetPathRole, QString::fromStdString(source));
//            layerItem->setData(DATA_COLUMN, common::LayerIDRole, i);
//            layerItem->setData(DATA_COLUMN, common::LayerTypeRole, QVariant::fromValue(layer->getGeomType()));

//            const char* layerName = layer->getName();
//            const char* displayName = "unknown";
//            if (layerName && layerName[0] != '\0')
//                displayName = layerName;
//            layerItem->setText(DATA_COLUMN, QString::fromUtf8(displayName));

//            if (auto layerStyle = layer->getStyle())
//            {
//                QIcon icon = QIcon();
//                emit updateLegendIconsRequired(layerStyle->getSymbol(), icon);
//                layerItem->setIcon(DATA_COLUMN, icon);
//            }
//            else
//                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/unknown.png"));
//        }
//        break;
//    }

//    case common::DatasetType::Raster:
//    {
//        auto rDs = static_cast<vrsa::raster::RasterDataset*>(dS);
//        mMapScene->addRasterDataset(rDs);
//        dSItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_RasterDataset));
//        dSItem->setData(DATA_COLUMN, common::DatasetTypeRole, QVariant::fromValue(common::DatasetType::Raster));
//        for (int i=0; i<rDs->channelsCount(); ++i)
//        {
//            auto& layer = rDs->getChannel(i);
//            //mMapScene->addLayer(layer);
//            auto* layerItem = new QTreeWidgetItem(dSItem);

//            layerItem->setCheckState(DATA_COLUMN,Qt::Checked);

//            layerItem->setData(DATA_COLUMN, common::ItemTypeRole, QVariant::fromValue(common::TreeItemType::ItemType_RasterLayer));
//            layerItem->setData(DATA_COLUMN, common::DatasetPathRole, QString::fromStdString(source));
//            layerItem->setData(DATA_COLUMN, common::LayerIDRole, i);

//            QString displayName = tr("Band №");
//            layerItem->setText(DATA_COLUMN, displayName + QString::number(i+1));
//            layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/rasterTreeItem.png"));

//        }
//        break;
//    }
//    default:
//        VRSA_WARNING("CORE", "Данный тип датасета пока не поддерживается");
//    }
//    mComps.mainLegendTree->addTopLevelItem(dSItem);
//    mComps.mainLegendTree->expandAll();
//    syncZOrderWithTree();



void vrsa::services::GISController::ApplyCRS(std::string name)
{  
    //    if (auto wktDesc = vrsa::georef::SpatialReferenceLibrary::getInstance().GetWKTByName(name))
    //        mProjCrs = vrsa::gdalwrapper::SpatialReference(*wktDesc, common::CRSDesctiptionFormat::WKT);
    //    else if (auto projDesc = vrsa::georef::SpatialReferenceLibrary::getInstance().GetProjByName(name))
    //        mProjCrs = vrsa::gdalwrapper::SpatialReference(*projDesc, common::CRSDesctiptionFormat::Proj);
    //    else if (auto epsgCode = vrsa::georef::SpatialReferenceLibrary::getInstance().GetEPSGByName(name))
    //        mProjCrs = vrsa::gdalwrapper::SpatialReference(std::to_string(*epsgCode), common::CRSDesctiptionFormat::EPSG);
    //    else
    //    {
    //        VRSA_ERROR("CORE", "Can't apply CRS:" + name);
    //        return;
    //    }
    //    emit projectCRSChanged(mProjCrs);
    //    //mMapCalculator->setCRS(mProjCrs);
}

bool vrsa::services::GISController::isCurrentCRSGeographic() const
{
    return mProjCrs.isGeographic();
}



void vrsa::services::GISController::syncZOrderWithTree() const
{
    int baseZ = common::MAX_Z_VALUE;
    for (int i=0; i<mComps.mainLegendTree->topLevelItemCount(); ++i)
    {
        auto topLevelItem = mComps.mainLegendTree->topLevelItem(i);
        auto& pM = mProjectManager;
        switch (topLevelItem->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>())
        {
        case common::ItemType_VectorDataset:
        {
            QString path = topLevelItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            for (auto& layer: static_cast<vector::VectorDataset*>(pM->getDatasetBySource(path.toStdString()))->getLayers())
                layer->setZValue(baseZ--);
            break;
        }
        case common::ItemType_RasterDataset:
        {
            QString path = topLevelItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            auto rasterDataset = static_cast<raster::RasterDataset*>(pM->getDatasetBySource(path.toStdString()));
            rasterDataset->setZValue(baseZ--);
            break;
        }
        default:
            break;
        }
    }
}

QIcon vrsa::services::GISController::getIconForDigitizingToolBtn(common::GeometryType type)
{
    switch (type)
    {
    case common::GeometryType::Point:
    case common::GeometryType::MultiPoint:
        return QIcon(":/images/icons/addPointFeature2.png");
    case common::GeometryType::LineString:
    case common::GeometryType::MultiLineString:
        return QIcon(":/images/icons/addLineFeature.png");
    case common::GeometryType::Polygon:
    case common::GeometryType::MultiPolygon:
        return QIcon(":/images/icons/addPolygonFeature.png");
    default:
        return QIcon(":/images/icons/unknown.png");
    }
}

QIcon vrsa::services::GISController::getIconForGeometryType(common::GeometryType type)
{
    switch (type)
    {
    case common::GeometryType::Point:
        return QIcon(":/images/icons/geometry/icon_point_bold_black.png");
    case common::GeometryType::MultiPoint:
        return QIcon(":/images/icons/geometry/icon_multi_points_gray.png");
    case common::GeometryType::LineString:
        return QIcon(":/images/icons/geometry/lineLayer.png");
    case common::GeometryType::MultiLineString:
        return QIcon(":/images/icons/geometry/icon_multi_lines_black.png");
    case common::GeometryType::Polygon:
        return QIcon(":/images/icons/geometry/polygonLayer.png");
    case common::GeometryType::MultiPolygon:
        return QIcon(":/images/icons/geometry/icon_multi_polygon_black.png");
    default:
        return QIcon(":/images/icons/unknown.png");
    }
}

void vrsa::services::GISController::showContextMenu(const QPoint &itemPos)
{
    qDebug()<<"DS COUNT" << mProjectManager->getDatasets().size();
    QTreeWidgetItem *clickedItem=mComps.mainLegendTree->itemAt(itemPos);
    if (!clickedItem)
        return;
    QMenu menu;
    switch (clickedItem->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>())
    {
    case common::ItemType_VectorDataset:
        qDebug() << "Vector Dataset Item Clicked!";
        menu.addAction(tr("Свойства"), this, [this, clickedItem]() {
            VectorDatasetForm form;
            form.setTreeItem(clickedItem);
            form.exec();
        });
        menu.addAction(tr("Закрыть датасет"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            auto vectorDataset = static_cast<vector::VectorDataset*>(mProjectManager->getDatasetBySource(path.toStdString()));
            mProjectManager->removeDataset(vectorDataset);
            delete clickedItem;

        });
        break;
    case common::ItemType_RasterDataset:
        qDebug() << "Raster Dataset Item Clicked!";
        menu.addAction(tr("Свойства"), this, [this, clickedItem]() {
            qDebug() << "properties";
        });
        menu.addAction(tr("Увеличить до слоя"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            int layerID = clickedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
            auto rasterDataset = static_cast<raster::RasterDataset*>(mProjectManager->getDatasetBySource(path.toStdString()));
            QRectF targetRect = rasterDataset->getBoundingBox();
            mComps.mapView->fitInView(targetRect, Qt::KeepAspectRatio);
            mComps.mapView->centerOn(targetRect.center());
            mComps.mapView->recalculateScale();
            qDebug() << targetRect;
        });
        menu.addAction(tr("Закрыть датасет"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            auto rasterDataset = static_cast<raster::RasterDataset*>(mProjectManager->getDatasetBySource(path.toStdString()));
            mProjectManager->removeDataset(rasterDataset);
            delete clickedItem;

        });
        break;
    case common::ItemType_VectorLayer:
        qDebug() << "Vector Layer Item Clicked!";

        menu.addAction(tr("Свойства"), this, [this, clickedItem]() {
            qDebug() << "properties";
        });

        menu.addAction(tr("Увеличить до слоя"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            int layerID = clickedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
            auto selectedLayer = mProjectManager->getLayer(path.toStdString(), layerID);
            QRectF targetRect = selectedLayer->getBoundingBox();
            mComps.mapView->fitInView(targetRect, Qt::KeepAspectRatio);
            mComps.mapView->centerOn(targetRect.center());
            mComps.mapView->recalculateScale();
            qDebug() << targetRect;
        });

        menu.addAction(tr("Таблица атрибутов"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            int layerID = clickedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
            auto selectedLayer = mProjectManager->getLayer(path.toStdString(), layerID);
            AttributeTableForm form(nullptr, selectedLayer);
            //VectorDatasetForm form;
            form.exec();
        });

        menu.addAction(tr("Настройки стиля"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            int layerID = clickedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
            auto selectedLayer = mProjectManager->getLayer(path.toStdString(), layerID);
            //            auto layerGeometryType = selectedLayer->getGeomType();
            //            auto style = selectedLayer->getStyle();
            //            if (!style)
            //                return;
            //            auto symbol = style->getSymbol();
            //            if (!symbol)
            //                return;
            VectorStylingForm form(selectedLayer);
            form.exec();
            QIcon icon = QIcon();
            emit updateLegendIconsRequired(selectedLayer->getStyle()->getSymbol(), icon);
            clickedItem->setIcon(DATA_COLUMN, icon);
        });

        break;
    case common::ItemType_RasterLayer:
        break;
    default:
        break;

    }
    menu.show();
    menu.exec(mComps.mainLegendTree->mapToGlobal(itemPos));

    //    if (clickedItem->parent()) {
    //        qDebug() << "Parent found:" << clickedItem->parent()->text(0);
    //        QTreeWidgetItem *parentItem = clickedItem->parent();
    //        QString parentPath = parentItem->toolTip(0);
    //        qDebug() << "Путь родителя:" << parentPath;
    //    }

}

void vrsa::services::GISController::showContextMenuPG(const QPoint &point)
{
    QTreeWidgetItem *clickedItem=mComps.postGisTree->itemAt(point);
    if (!clickedItem)
        return;
    QMenu menu;
    switch (clickedItem->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>())
    {
    case common::ItemType_VectorDataset:
        menu.addAction(tr("Свойства"), this, [this, clickedItem]() {
            VectorDatasetForm form;
            form.setTreeItem(clickedItem);
            form.exec();
        });
        break;
    case common::ItemType_VectorLayer:
        menu.addAction(tr("Добавить в проект"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            int layerID = clickedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
            auto selectedLayer = mProjectManager->getLayer(path.toStdString(), layerID);
            if (selectedLayer)
                mProjectManager->addPostGISLayer(selectedLayer);
        });
        break;
    default:
        break;

    }
    menu.show();
    menu.exec(mComps.postGisTree->mapToGlobal(point));

}

void vrsa::services::GISController::showContextMenuLOD(const QPoint &point)
{
    QMenu menu;
    QActionGroup* group = new QActionGroup(&menu);
    group->setExclusive(true);

    QAction* optionDouglas = menu.addAction(tr("Использовать алгоритм Дугласа-Пекера"));
    optionDouglas->setCheckable(true);
    optionDouglas->setActionGroup(group);

    QAction* optionTopo = menu.addAction("Использовать алгоритм, сохраняющий топологию");
    optionTopo->setCheckable(true);
    optionTopo->setActionGroup(group);

    auto currentAlgo = common::ApplicationSettings::instance().getLodAlgorithm();
    optionDouglas->setChecked(currentAlgo == common::LodAlgorithmType::DouglasPeucker);
    optionTopo->setChecked(currentAlgo == common::LodAlgorithmType::TopologyPreservingSimplifier);

    connect(optionDouglas, &QAction::triggered, this, [](bool enable){
        if (enable)
            common::ApplicationSettings::instance().setLodAlgorithm(common::LodAlgorithmType::DouglasPeucker);
    });
    connect(optionTopo, &QAction::triggered, this, [](bool enable){
        if (enable)
            common::ApplicationSettings::instance().setLodAlgorithm
                    (common::LodAlgorithmType::TopologyPreservingSimplifier);
    });
    menu.show();
    menu.exec(mComps.lodBtn->mapToGlobal(point));
}

void vrsa::services::GISController::onLayerTreeDataChanged(QTreeWidgetItem *selectedItem, int column)
{
    auto& pM = mProjectManager;
    switch (selectedItem->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>())
    {
    case common::ItemType_VectorDataset:
    {
        QString path = selectedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
        for (auto& layer: static_cast<vector::VectorDataset*>(pM->getDatasetBySource(path.toStdString()))->getLayers())
        {
            layer->setVisible(selectedItem->checkState(column));
        }
        for (int i = 0; i < selectedItem->childCount(); ++i)
        {
            QTreeWidgetItem* child = selectedItem->child(i);
            child->setCheckState(column, selectedItem->checkState(column));
        }
        break;
    }
    case common::ItemType_RasterDataset:
        break;
    case common::ItemType_VectorLayer:
    {
        QString path = selectedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
        int id = selectedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
        auto selectedLayer = pM->getLayer(path.toStdString(), id);
        if (selectedLayer)
            selectedLayer->setVisible(selectedItem->checkState(column));
        break;
    }
    case common::ItemType_RasterLayer:
        break;
    default:
        break;

    }

}

void vrsa::services::GISController::onLayerTreeItemDoubleClicked(QTreeWidgetItem *selectedItem, int column)
{
    auto& pM = mProjectManager;
    switch (selectedItem->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>())
    {
    case common::ItemType_VectorDataset:
        return;

    case common::ItemType_RasterDataset:
        break;
    case common::ItemType_VectorLayer:
    {
        QString path = selectedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
        int id = selectedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
        auto selectedLayer = pM->getLayer(path.toStdString(), id);
        if (selectedLayer)
        {

            pM->setActiveVectorLayer(selectedLayer);
            qDebug()<< "Active vector layer changed:" << pM->getActiveVectorLayer();
            emit activeLayerChanged(getIconForDigitizingToolBtn(selectedLayer->getGeomType()));
            emit activeLayerChanged(selectedLayer->getName());
            emit activeLayerChanged(selectedLayer);
        }
        break;
    }
    case common::ItemType_RasterLayer:
        break;
    default:
        break;

    }
}
//treewidget drag n drop
void vrsa::services::GISController::onItemDragRequested(QDragMoveEvent *event, bool *accepted)
{
    *accepted = true;
    //qDebug()<<"item drag request !";
}

void vrsa::services::GISController::onItemDropped(QDropEvent* event, bool *accepted)
{

    QTreeWidgetItem* source = mComps.mainLegendTree->currentItem();
    QTreeWidgetItem* target = mComps.mainLegendTree->itemAt(event->pos());
    if (!source || source == target)
    {
        *accepted = false;
        return;
    }
    auto& pM = mProjectManager;
    qDebug() << "=== DROP DEBUG ===";
    qDebug() << "source:" << source << "text:" << (source ? source->text(0) : "null");
    qDebug() << "target:" << target << "text:" << (target ? target->text(0) : "null");

    auto sourceType = source->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>();
    qDebug() << "source type:" << static_cast<int>(sourceType);
    switch (source->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>()) //Что перемещаем
    {
    case common::TreeItemType::ItemType_VectorDataset:
    case common::TreeItemType::ItemType_RasterDataset:
    {
        if (!target) //если нет таргета то просто разрешаем перемещение
            break;

        //если есть таргет
        QRect targetRect = mComps.mainLegendTree->visualItemRect(target);
        int cursorY = event->pos().y();

        //получаем toplevelitem target'a
        QTreeWidgetItem* rootTarget = target;
        while (rootTarget->parent())
        {
            rootTarget = rootTarget->parent();
        }
        int sourceIndex = mComps.mainLegendTree->indexOfTopLevelItem(source); //всегда dataset должен быть корнем ??
        int rootTargetIndex = mComps.mainLegendTree->indexOfTopLevelItem(rootTarget);
        qDebug()<<"Индекс корня чего вставляем- " << sourceIndex;
        qDebug()<<"Индекс корня куда вставляем - " << rootTargetIndex;

        if (sourceIndex > rootTargetIndex) //если перемещаемый стоит ниже того на чье место перемещаем
        {
            if (cursorY < targetRect.center().y()) // верхняя половина — вставить перед
            {
                qDebug() << "=== BEFORE MOVE ===";
                qDebug() << "target text:" << target->text(0);
                qDebug() << "target parent:" << (target->parent() ? target->parent()->text(0) : "none");
                qDebug() << "rootTarget text:" << rootTarget->text(0);
                qDebug() << "rootTargetIndex:" << rootTargetIndex;
                qDebug() << "sourceIndex:" << sourceIndex;

                QTreeWidgetItem* temp1 = mComps.mainLegendTree->takeTopLevelItem(sourceIndex); //убираем то что перемещаем

                qDebug() << "After take source - items:";
                for (int i = 0; i < mComps.mainLegendTree->topLevelItemCount(); ++i) {
                    qDebug() << "  " << i << ":" << mComps.mainLegendTree->topLevelItem(i)->text(0);
                }
                rootTargetIndex = mComps.mainLegendTree->indexOfTopLevelItem(rootTarget);
                QTreeWidgetItem* temp2 = mComps.mainLegendTree->takeTopLevelItem(rootTargetIndex);

                qDebug() << "After both takes - items:";
                for (int i = 0; i < mComps.mainLegendTree->topLevelItemCount(); ++i) {
                    qDebug() << "  " << i << ":" << mComps.mainLegendTree->topLevelItem(i)->text(0);
                }

                mComps.mainLegendTree->insertTopLevelItem(rootTargetIndex, temp1);
                mComps.mainLegendTree->insertTopLevelItem(rootTargetIndex+1,
                                                          temp2);

                qDebug() << "After inserts - items:";
                for (int i = 0; i < mComps.mainLegendTree->topLevelItemCount(); ++i) {
                    qDebug() << "  " << i << ":" << mComps.mainLegendTree->topLevelItem(i)->text(0);
                }

                mComps.mainLegendTree->update();  // не всегда помогает
                mComps.mainLegendTree->doItemsLayout();  // принудительная перерисовка

                *accepted=false;
            }
            else
            {
                QTreeWidgetItem* temp1 = mComps.mainLegendTree->takeTopLevelItem(sourceIndex); //убираем то что перемещаем

                mComps.mainLegendTree->insertTopLevelItem(rootTargetIndex+1, temp1);
                *accepted=false;
            }
        }
        else //если перемещаемый стоит выше того на чье место перемещаем
        {
            if (cursorY < targetRect.center().y()) // верхняя половина — вставить перед
            {
                QTreeWidgetItem* temp1 = mComps.mainLegendTree->takeTopLevelItem(sourceIndex); //убираем то что перемещаем
                //QTreeWidgetItem* temp2 = mComps.mainLegendTree->takeTopLevelItem(rootTargetIndex);
                rootTargetIndex = mComps.mainLegendTree->indexOfTopLevelItem(rootTarget);
                mComps.mainLegendTree->insertTopLevelItem(rootTargetIndex, temp1);
                //mComps.mainLegendTree->insertTopLevelItem(sourceIndex, temp2);
                *accepted=true;
            }
            else
            {
                QTreeWidgetItem* temp1 = mComps.mainLegendTree->takeTopLevelItem(sourceIndex); //убираем то что перемещаем
                rootTargetIndex = mComps.mainLegendTree->indexOfTopLevelItem(rootTarget);
                mComps.mainLegendTree->insertTopLevelItem(rootTargetIndex+1, temp1);
                *accepted=true;
            }
        }
        break;
    }
    case common::ItemType_VectorLayer:
    {
        //пока так, в теории их можно перемещать внутри датасет айтема, если слои комплексные,
        //но лучше вообще сделать для них разные корни и делить по типу геометрии
        *accepted = false;
        break;
    }
    case common::ItemType_RasterLayer:
    {
        *accepted = false;
        break;
    }
    default:
        qDebug() << "tf r u";
        break;
    }

    qDebug()<<"item dropped !";
    qDebug()<<"source - " << source;
    qDebug()<<"target - " << target;
    qDebug()<<"Accepted:" << accepted;
    syncZOrderWithTree();

}


void vrsa::services::GISController::startDigitizing() //onpbAddFeature
{
    auto& pM = mProjectManager;
    auto activeLayer = pM->getActiveVectorLayer();
    if (!activeLayer)
    {
        //emit activeLayerChanged(getIconForDigitizingToolBtn(common::GeometryType::Unknown));//for digitizing tool button
        VRSA_DEBUG("CORE", "Please choose active layer before digitizing");
        return;
    }
    switch (activeLayer->getGeomType())
    {
    case common::GeometryType::Point:
        addMapTool(common::MapToolType::PointDigitizingTool, activeLayer);
        break;
    case common::GeometryType::LineString:
        addMapTool(common::MapToolType::LineStringDigitizingTool, activeLayer);
        break;
    case common::GeometryType::Polygon:
        addMapTool(common::MapToolType::PolygonDigitizingTool, activeLayer);
        break;
    default:
        VRSA_DEBUG("CORE", "Digitizing for active layer geometry type currently not supported");
    }



}



void vrsa::services::GISController::addMapTool(common::MapToolType type, vector::VectorLayer* layer)
{
    mMapScene->deselectCurrentMapTool();
    auto tool = tools::MapToolFactory::createForScene(mMapScene, type, layer);
    if (tool)
    {
        connect(tool.get(), &tools::MapTool::toolEvent, this, &GISController::onToolEvent);
        connect(tool.get(), &tools::MapTool::statusMessage, mComps.statusBar, &QStatusBar::showMessage);
        mMapScene->setMapTool(std::move(tool));
    }

}


void vrsa::services::GISController::removeMapTool()
{
    mMapScene->deselectCurrentMapTool();
}

void vrsa::services::GISController::onSingleSelectionToolClicked(bool checked)
{
    qDebug()<<"single";
    addMapTool(common::MapToolType::SingleSelectionTool);
}

void vrsa::services::GISController::onRectSelectionToolClicked(bool checked)
{
    qDebug()<<"rect";
    addMapTool(common::MapToolType::RectSelectionTool);
}

void vrsa::services::GISController::onGeometryEditToolClicked(bool checked)
{
    qDebug()<<"geom edit";
    //    if (checked)
    addMapTool(common::MapToolType::EditGeometryTool);
    //    else
    //        mMapScene->deselectCurrentMapTool();
}

void vrsa::services::GISController::onDigitizingToolClicked(bool checked)
{
    if (checked)
        startDigitizing();
    else
        removeMapTool();
}

void vrsa::services::GISController::onRulerToolClicked(bool checked)
{
    qDebug()<<"ruler tool";
    addMapTool(common::MapToolType::RulerTool);
}

void vrsa::services::GISController::onCRSComboBoxIndexChanged(int index)
{
    auto crs = spatialref::SpatialReferenceDatabase::instance().createFromIndex(index);
    if (crs.isValid())
    {
        //auto crsInfo = spatialref::SpatialReferenceDatabase::instance().getCRSInfoByEPSG(std::stoi(crs.getAuthorityCode()));
        VRSA_DEBUG("CORE", "Project CRS changed to:" + mComps.crsCombo->currentText().toStdString());
        mProjCrs = crs;
        emit projectCRSChanged(crs);
        return;
    }
    VRSA_DEBUG("CORE", "Invalid CRS set");
}

void vrsa::services::GISController::onToolEvent(tools::MapTool::ToolEventType type, const QVariant &data)
{
    using namespace vrsa::tools;
    switch (type)
    {
    case MapTool::ToolEventType::FeatureSelected:
    {
        if (auto selectedFeatureGraphicsItem = data.value<graphics::FeatureGraphicsItem*>())
            handleFeatureSelected(selectedFeatureGraphicsItem, true);
        break;
    }
    case MapTool::ToolEventType::MultipleFeaturesSelected:
        if (data.canConvert<std::vector<graphics::FeatureGraphicsItem*>>())
            handleMultipleFeaturesSelected(data.value<std::vector<graphics::FeatureGraphicsItem*>>());
    case MapTool::ToolEventType::SimpleLineRulerGeometryChanged:
    {
        if (data.canConvert<std::vector<QPointF>>())
        {
            auto points = data.value<std::vector<QPointF>>();
            auto startPoint = points[0];
            auto endPoint   = points[1];
            if (!mComps.measurementForm)
                mComps.measurementForm = new MeasurementForm();

            mComps.measurementForm->show();
            connect(mComps.measurementForm, &MeasurementForm::ellipsoidChanged, this, [this](const std::string& name){
                auto [startPoint, endPoint] = mComps.measurementForm->getPoints();
                double distance = calculations::MapCalculator::calculateDistance(
                    startPoint, endPoint, calculations::MeasurementType::Geodesic, name);
                mComps.measurementForm->setDistance(distance);
            });
            connect(mComps.measurementForm, &MeasurementForm::modeChanged, this, [this](){
                auto [startPoint, endPoint] = mComps.measurementForm->getPoints();
                double distance;
                if (mComps.measurementForm->isGeodesicalMode())
                {
                    distance = calculations::MapCalculator::calculateDistance(
                        startPoint, endPoint, calculations::MeasurementType::Geodesic,
                        mComps.measurementForm->getEllipsoid());
                }
                else
                    distance = calculations::MapCalculator::calculateDistance(startPoint, endPoint);
                mComps.measurementForm->setDistance(distance);
            });
            //удаляется при закрытии
            connect(mComps.measurementForm,  &QObject::destroyed, this, [this]() {
                mComps.measurementForm = nullptr;
                removeMapTool();
            });
            mComps.measurementForm->setStartPoint(startPoint);
            mComps.measurementForm->setEndPoint(endPoint);
            double distance = -1.0;
            if (mComps.measurementForm->isGeodesicalMode())
            {
                distance = calculations::MapCalculator::calculateDistance(
                    startPoint, endPoint, calculations::MeasurementType::Geodesic,
                    mComps.measurementForm->getEllipsoid());
            }
            else
                distance = calculations::MapCalculator::calculateDistance(startPoint, endPoint);
            mComps.measurementForm->setDistance(distance);
        }
        break;
    }

    default:
        break;
    }
}

void vrsa::services::GISController::handleFeatureSelected(graphics::FeatureGraphicsItem *item, bool shouldClearTree)
{
    auto selectionTreeWidget = mComps.featureSelectionTree;
    if (shouldClearTree)
        selectionTreeWidget->clear();
    if (!item)
        return;
    auto feature = item->getFeature();
    auto layer = mProjectManager->getLayerAssociatedWithFeature(feature);
    if (!layer)
        return;
    QTreeWidgetItem* root = new QTreeWidgetItem;
    root->setText(DATA_COLUMN, layer->getName());
    QString tooltipText = QString("<b>Слой:</b> %1<br><b>FID:</b> %2")
            .arg(layer->getName())
            .arg(QString::number(feature->getFID())); // предположим, у layer есть такой метод

    // Устанавливаем ToolTip для первого столбца [citation:1][citation:3]
    root->setToolTip(DATA_COLUMN, tooltipText);

    mComps.statusBar->showMessage("Веторный объект слоя: "+QString::fromStdString(layer->getNameAsString())
                                  +" | FID:"+ QString::number(feature->getFID()), 5000);

    selectionTreeWidget->addTopLevelItem(root);
    for (const auto& attr: feature->getAttributesAsQString())
    {
        QTreeWidgetItem* child = new QTreeWidgetItem;
        child->setText(DATA_COLUMN, attr.first + ":" + attr.second);
        root->addChild(child);
    }


}

void vrsa::services::GISController::handleMultipleFeaturesSelected(const std::vector<graphics::FeatureGraphicsItem *>& items)
{
    mComps.featureSelectionTree->clear();
    for (const auto& item: items)
        handleFeatureSelected(item, false);
}

//===============================================================================================
//===============================================================================================
//=============================СЛОТЫ ОБРАБОТЧИКИ СИГНАЛОВ СО СЦЕНЫ===============================
//===============================================================================================
//===============================================================================================
void vrsa::services::GISController::onMapHolderScaleChanged(int mapScale, double widgetScale)
{
    mComps.scaleEdit->setText(QString::number(mapScale));
}

void vrsa::services::GISController::onMouseCoordinatesChanged(const QPointF& p)
{
    if (mProjCrs.isGeographic())
    {
        QString xCoord = QString::number(p.x(), 'f', 5); // форматируем координаты с пятью знаками после запятой
        QString yCoord = QString::number(p.y(), 'f', 5);
        QString coords = QString("%1  %2").arg(xCoord, yCoord);
        mComps.coordEdit->setText(coords);
    }
    else
    {
        QString xCoord = QString::number(p.x(), 'f', 2);
        QString yCoord = QString::number(p.y(), 'f', 2);
        QString coords = QString("%1  %2").arg(xCoord, yCoord);
        mComps.coordEdit->setText(coords);
    }
}

void vrsa::services::GISController::onActiveLayerChanged(const QIcon &icon)
{
    mComps.addFeatureBtn->setIcon(icon);
    mComps.mapToolsGrp->setExclusive(false);
    mComps.addFeatureBtn->setChecked(false);
    mComps.mapToolsGrp->setExclusive(true);
    removeMapTool();
    mComps.addFeatureBtn->setAttribute(Qt::WA_UnderMouse, false);
}

void vrsa::services::GISController::onActiveLayerChanged(const QString &name)
{
    mComps.activeLayerLabel->setText(tr("Активный слой:") + name);
}


//===============================================================================================
//===============================================================================================
//=================================СЛОТЫ ОБРАБОТЧИКИ ACTION======================================
//===============================================================================================
//===============================================================================================

void vrsa::services::GISController::onOpenLayerActionTriggered()
{
    std::string fileName=QFileDialog::getOpenFileName(nullptr,"Chose Vector Layer","Vector Files").toStdString();
    emit datasetReadingRequested(fileName); //сигнал для project manager
}

void vrsa::services::GISController::onCreateLayerActionTriggered()
{
    VectorLayerCreationForm form;
    connect(&form, &VectorLayerCreationForm::layerCreationAccepted, this,
            &GISController::onVectorLayerCreationAccepted);
    form.exec();
}

void vrsa::services::GISController::onCreatePointLayerActionTriggered()
{
    VectorLayerCreationForm form(common::GeometryType::Point);
    connect(&form, &VectorLayerCreationForm::layerCreationAccepted, this,
            &GISController::onVectorLayerCreationAccepted);
    form.exec();
}

void vrsa::services::GISController::onCreateLineLayerActionTriggered()
{
    VectorLayerCreationForm form(common::GeometryType::LineString);
    connect(&form, &VectorLayerCreationForm::layerCreationAccepted, this,
            &GISController::onVectorLayerCreationAccepted);
    form.exec();
}

void vrsa::services::GISController::onCreatePolygonLayerActionTriggered()
{
    VectorLayerCreationForm form(common::GeometryType::Polygon);
    connect(&form, &VectorLayerCreationForm::layerCreationAccepted, this,
            &GISController::onVectorLayerCreationAccepted);
    form.exec();
}

void vrsa::services::GISController::onCreateBufferActionTriggered()
{
    auto projectedLayers = mProjectManager->getProjectedVectorLayers();
    std::vector<std::string> layerNames;
    for (const auto& layer: projectedLayers)
        layerNames.emplace_back(layer->getNameAsString());
    GeosOperationForm form(vector::SpatialOperationType::Buffer, layerNames);
    connect(&form, &GeosOperationForm::operationAccepted, this, [this](const common::SpatialOperationDTO& dto){
        auto layerName = dto.firstInputLayerName;
        auto layer = mProjectManager->getVectorLayerByName(layerName);
        auto operation = std::make_unique<vector::BufferOperation>(layer, dto, mVectorCreator.get());
        operation->calculate();
    });
    form.exec();
}

void vrsa::services::GISController::onCreateTriangulationActionTriggered()
{
    auto projectedLayers = mProjectManager->getProjectedVectorLayers();
    std::vector<std::string> layerNames;
    for (const auto& layer: projectedLayers)
        layerNames.emplace_back(layer->getNameAsString());

    GeosOperationForm form(vector::SpatialOperationType::Triangulation, layerNames);
    connect(&form, &GeosOperationForm::operationAccepted, this, [this](const common::SpatialOperationDTO& dto){
        auto layerName = dto.firstInputLayerName;
        auto layer = mProjectManager->getVectorLayerByName(layerName);
        auto operation = std::make_unique<vector::TriangulationOperation>(layer, dto, mVectorCreator.get());
        operation->calculate();
    });
    form.exec();
}

void vrsa::services::GISController::onCreateVoronoiActionTriggered()
{
    auto projectedLayers = mProjectManager->getProjectedVectorLayers();
    std::vector<std::string> layerNames;
    for (const auto& layer: projectedLayers)
        layerNames.emplace_back(layer->getNameAsString());
    GeosOperationForm form(vector::SpatialOperationType::VoronoiDiagramm, layerNames);
    connect(&form, &GeosOperationForm::operationAccepted, this, [this](const common::SpatialOperationDTO& dto){
        auto layerName = dto.firstInputLayerName;
        auto layer = mProjectManager->getVectorLayerByName(layerName);
        auto operation = std::make_unique<vector::VoronoiOperation>(layer, dto, mVectorCreator.get());
        operation->calculate();
    });
    form.exec();
}

void vrsa::services::GISController::onCreateIntersectionActionTriggered()
{
//    emit datasetReadingRequested("/home/doger/Documents/GepPortal/geosSpatiaTest/overlayData/intersectionKostrm.shp");
//    emit datasetReadingRequested("/home/doger/Documents/GepPortal/geosSpatiaTest/overlayData/kostrm.shp");
    auto projectedLayers = mProjectManager->getProjectedVectorLayers();
    std::vector<std::string> layerNames;
    for (const auto& layer: projectedLayers)
        layerNames.emplace_back(layer->getNameAsString());
    GeosOperationForm form(vector::SpatialOperationType::Intersection, layerNames);
    connect(&form, &GeosOperationForm::operationAccepted, this, [this](const common::SpatialOperationDTO& dto){
        auto layerName = dto.firstInputLayerName;
        auto overlayLayerName = dto.secondInputLayerName;
        auto layer = mProjectManager->getVectorLayerByName(layerName);
        auto overlayLayer = mProjectManager->getVectorLayerByName(overlayLayerName);
        auto operation = std::make_unique<vector::IntersectionOperation>(layer, overlayLayer,
                                                                         dto, mVectorCreator.get());
        operation->calculate();
    });
    form.exec();
}

void vrsa::services::GISController::onCreateUnionActionTriggered()
{
    auto projectedLayers = mProjectManager->getProjectedVectorLayers();
    std::vector<std::string> layerNames;
    for (const auto& layer: projectedLayers)
        layerNames.emplace_back(layer->getNameAsString());
    GeosOperationForm form(vector::SpatialOperationType::Union, layerNames);
    connect(&form, &GeosOperationForm::operationAccepted, this, [this](const common::SpatialOperationDTO& dto){
        auto layerName = dto.firstInputLayerName;
        auto overlayLayerName = dto.secondInputLayerName;
        auto layer = mProjectManager->getVectorLayerByName(layerName);
        auto overlayLayer = mProjectManager->getVectorLayerByName(overlayLayerName);
        auto operation = std::make_unique<vector::UnionOperation>(layer, overlayLayer,
                                                                         dto, mVectorCreator.get());
        operation->calculate();
    });
    form.exec();
}

void vrsa::services::GISController::onCreateDifferenceActionTriggered()
{
    emit datasetReadingRequested("/home/doger/Documents/GepPortal/geosSpatiaTest/overlayData/intersectionKostrm.shp");
    emit datasetReadingRequested("/home/doger/Documents/GepPortal/geosSpatiaTest/overlayData/kostrm.shp");
    auto projectedLayers = mProjectManager->getProjectedVectorLayers();
    std::vector<std::string> layerNames;
    for (const auto& layer: projectedLayers)
        layerNames.emplace_back(layer->getNameAsString());
    GeosOperationForm form(vector::SpatialOperationType::Difference, layerNames);
    connect(&form, &GeosOperationForm::operationAccepted, this, [this](const common::SpatialOperationDTO& dto){
        auto layerName = dto.firstInputLayerName;
        auto overlayLayerName = dto.secondInputLayerName;
        auto layer = mProjectManager->getVectorLayerByName(layerName);
        auto overlayLayer = mProjectManager->getVectorLayerByName(overlayLayerName);
        auto operation = std::make_unique<vector::DifferenceOperation>(layer, overlayLayer,
                                                                         dto, mVectorCreator.get());
        operation->calculate();
    });
    form.exec();
}

void vrsa::services::GISController::onCreateSymDifferenceActionTriggered()
{
    emit datasetReadingRequested("/home/doger/Documents/GepPortal/geosSpatiaTest/overlayData/intersectionKostrm.shp");
    emit datasetReadingRequested("/home/doger/Documents/GepPortal/geosSpatiaTest/overlayData/kostrm.shp");
    auto projectedLayers = mProjectManager->getProjectedVectorLayers();
    std::vector<std::string> layerNames;
    for (const auto& layer: projectedLayers)
        layerNames.emplace_back(layer->getNameAsString());
    GeosOperationForm form(vector::SpatialOperationType::SymDifference, layerNames);
    connect(&form, &GeosOperationForm::operationAccepted, this, [this](const common::SpatialOperationDTO& dto){
        auto layerName = dto.firstInputLayerName;
        auto overlayLayerName = dto.secondInputLayerName;
        auto layer = mProjectManager->getVectorLayerByName(layerName);
        auto overlayLayer = mProjectManager->getVectorLayerByName(overlayLayerName);
        auto operation = std::make_unique<vector::SymDifferenceOperation>(layer, overlayLayer,
                                                                         dto, mVectorCreator.get());
        operation->calculate();
    });
    form.exec();
}

void vrsa::services::GISController::onReprojectVectorActionTriggered()
{
    auto layerNames = mProjectManager->getVectorLayersNames();
    if (layerNames.empty())
    {
        VRSA_WARNING("CORE", "No vector layers available. Please add a vector layer first.");
        return;
    }
    VectorReprojectionForm form(layerNames);
    connect(&form, &VectorReprojectionForm::reprojectingAccepted, this, [this](const std::string& layer,
                                                                               const std::string& crs,
                                                                               const std::string& path){
        qDebug()<<QString::fromStdString(layer);
        qDebug()<<QString::fromStdString(crs);
        qDebug()<<QString::fromStdString(path);
        auto vectorLayer = mProjectManager->getVectorLayerByName(layer);
        auto dstCrs = spatialref::SpatialReferenceDatabase::instance().createFromName(crs);
        mVectorCreator->reprojectVectorLayer(vectorLayer, dstCrs, path);
    });
    form.exec();
}

void vrsa::services::GISController::onOpenRasterLayerActionTriggered()
{
    std::string fileName=QFileDialog::getOpenFileName(nullptr,"Chose Raster Layer","Raster Files").toStdString();
    emit datasetReadingRequested(fileName);
}

void vrsa::services::GISController::prepareRasterProcessingParams(common::RasterProcessingParams &params)
{
    auto dstSpRef = spatialref::SpatialReferenceDatabase::instance().createFromName(params.targetCRS.value());
    std::string name = dstSpRef.getAuthorityName();
    std::string code = dstSpRef.getAuthorityCode();
    if (!name.empty() && !code.empty())
        params.epsg = name +":"+ code;
    std::string wkt = dstSpRef.toWKT();
    if (!wkt.empty())
        params.wkt = wkt;
    std::string proj = dstSpRef.toProj();
    if (!proj.empty())
        params.proj = proj;
}


void vrsa::services::GISController::onReprojectRasterLayerActionTriggered()
{
    RasterProccessingForm form(common::RasterOperationType::Reproject, mProjectManager->getRasterDatasetSources());
    connect(&form, &RasterProccessingForm::operationAccepted, this, [this](common::RasterProcessingParams params)
    {
        prepareRasterProcessingParams(params);
        auto proccessor = raster::ReprojectProcessor(params);
        if (proccessor.execute())
            emit datasetReadingRequested(params.outputPath);
    });
    form.exec();
}

void vrsa::services::GISController::onCutRasterByVectorMaskActionTriggered()
{
    RasterProccessingForm form(common::RasterOperationType::CutByVectorMask, mProjectManager->getRasterDatasetSources(),
                               mProjectManager->getVectorDatasetSources());
    connect(&form, &RasterProccessingForm::operationAccepted, this, [this](common::RasterProcessingParams params)
    {
        prepareRasterProcessingParams(params);
        auto proccessor = raster::ClipProcessor(params);
        if (proccessor.execute())
            emit datasetReadingRequested(params.outputPath);
    });
    form.exec();

}

void vrsa::services::GISController::onCreateIsolinesActionTriggered()
{
    RasterProccessingForm form(common::RasterOperationType::Isolines, mProjectManager->getRasterDatasetSources());
    connect(&form, &RasterProccessingForm::operationAccepted, this, [this](common::RasterProcessingParams params)
    {
        prepareRasterProcessingParams(params);
        auto proccessor = raster::IsolinesProcessor(params);
        if (proccessor.execute())
            emit datasetReadingRequested(params.outputPath);
    });
    form.exec();
}




void vrsa::services::GISController::onWMSConnectionTriggered()
{

    //     std::string osmConfig = R"(<GDAL_WMS>
    //    <Service name="TMS">
    //        <ServerUrl>https://tile.openstreetmap.org/${z}/${x}/${y}.png</ServerUrl>
    //    </Service>
    //    <DataWindow>
    //        <UpperLeftX>-20037508.34</UpperLeftX>
    //        <UpperLeftY>20037508.34</UpperLeftY>
    //        <LowerRightX>20037508.34</LowerRightX>
    //        <LowerRightY>-20037508.34</LowerRightY>
    //        <TileLevel>18</TileLevel>
    //        <TileCountX>1</TileCountX>
    //        <TileCountY>1</TileCountY>
    //        <YOrigin>top</YOrigin>
    //    </DataWindow>
    //    <Projection>EPSG:3857</Projection>
    //    <BlockSizeX>256</BlockSizeX>
    //    <BlockSizeY>256</BlockSizeY>
    //    <BandsCount>3</BandsCount>
    //    <Cache/>
    //</GDAL_WMS>)";
}

void vrsa::services::GISController::onXYZConnectionTriggered()
{
    MapServiceConnectionForm form(common::WebServiceType::XYZ, nullptr);

    connect(&form, &MapServiceConnectionForm::testConnectionRequested, this, [&form](const std::string& str){
        bool valid = gdalwrapper::GDALReader::validateTMSConnection(str);
        form.onConnectionValidated(valid, str);
    });
    connect(&form, &MapServiceConnectionForm::connectionXMLReady, this,
            [this](const QString& connectionName, const std::string& str) {
        mProjectManager->createXYZConnection(connectionName, str);
    });
    form.exec();
}

void vrsa::services::GISController::onPostGisConnectionActionTriggered()
{
    PostGISConnectionForm form;
    connect(&form, &PostGISConnectionForm::testConnectionRequested, this, [&form](const std::string& str){
        bool valid = gdalwrapper::GDALReader::validatePostGisConnectionString(str);
        form.onConnectionStringValidated(valid, str);
    });
    connect(&form, &PostGISConnectionForm::connectionStringReady, this,
            [this](const QString& connectionName, const std::string& str) {
        mProjectManager->createPostGISConnection(connectionName, str);
    });
    form.exec();
}



//получение описания векторного слоя с диалогового окна по его созданию
void vrsa::services::GISController::onVectorLayerCreationAccepted(const common::LayerDefinition &layerDef)
{
    if (!layerDef.isValid())
    {
        VRSA_ERROR("CORE", "Can't create new vector layer. Reason: invalid layer definition");
        return;
    }
    vectorLayerCreationRequested(layerDef);

}


