#include "giscontroller.h"
vrsa::services::GISController::GISController(QObject *parent)
    : QObject{parent},
      mMapView{nullptr},
      mDigitizingManager{std::make_unique<DigitizingManager>()}
      //mMapCalculator{std::make_unique<vrsa::calculations::MapCalculator>()}
      //mCRSLib{std::make_unique<vrsa::georef::SpatialReferenceLibrary>()}
{

}

void vrsa::services::GISController::initializeScene(MapHolder *view)
{
    if (!view)
    {
        VRSA_ERROR("MapScene initialization", "Nullptr QGraphicsView");
        return;
    }
    mMapView = view;
    mMapView->connect(this, &vrsa::services::GISController::ProjectCRSChanged, mMapView, &MapHolder::onCrsChanged);
    mMapScene = new vrsa::graphics::MapScene();
    mMapView->setScene(mMapScene);
    connect(mMapView, &MapHolder::scaleChanged, mMapScene, &graphics::MapScene::onMapHolderScaleChanged);
    mMapView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    mMapView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    mMapView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    view->setOptimizationFlags(
        QGraphicsView::DontSavePainterState |
        QGraphicsView::DontAdjustForAntialiasing
    );

    view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setRenderHint(QPainter::SmoothPixmapTransform, false);

    // Отключаем кэширование фона
    view->setCacheMode(QGraphicsView::CacheNone);
    // ui->graphicsView->scene()->setSceneRect(-180,90,360,-180);
    double xMin = -20037508.34;
    double yMax = 20037508.34;
    double width = 40075016.68;
    double height = 20037508.34;

    // Установка сцены в пределах EPSG:3857 координат
    mMapView->scale(1,-1);
    mMapView->setSceneRect(xMin*4, 4*yMax, 4*width, -8*height);
    mMapView->recalculateScale();
}

void vrsa::services::GISController::setTreeWidget(TreeWidget *treeWidget)
{
    mTreeWidget = treeWidget;
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mTreeWidget, &TreeWidget::customContextMenuRequested,
           this, &services::GISController::showContextMenu);

    connect(mTreeWidget, &TreeWidget::itemChanged,
            this, &services::GISController::layerTreeDataChanged);

    connect(mTreeWidget, &TreeWidget::itemDoubleClicked,
            this, &services::GISController::onLayerTreeItemDoubleClicked);
//    connect(mTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
//            this, SLOT(showContextMenu(const QPoint&)));
}

void vrsa::services::GISController::LoadDataSet(std::string &source)
{
    vrsa::gdalwrapper::GDALReader reader;
    std::unique_ptr<vrsa::gdalwrapper::Dataset> ptr;
    try
    {
        ptr=reader.readDataset(source);
    }
    catch (const vrsa::common::DataSetOpenException& e)
    {
        VRSA_DEBUG("DataSetOpenException", e.what());
    }
    if (!ptr)
    {
        VRSA_ERROR("GDAL", "Can't read dataset: " + source);
        return;
    }
    auto& projManager = services::ProjectManager::instance();
    auto dS = ptr.get();
    projManager.AddDataset(std::move(ptr));

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
        for (int i=0; i<vDs->layersCount(); ++i)
        {
            auto& layer = vDs->getLayer(i);
            mMapScene->addLayer(layer);
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
            switch (layer->getGeomType())
            {
            case common::GeometryType::Point:
            case common::GeometryType::MultiPoint:
                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/pointLayer.png"));
                break;
            case common::GeometryType::LineString:
            case common::GeometryType::MultiLineString:
                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/lineLayer.png"));
                break;
            case common::GeometryType::Polygon:
            case common::GeometryType::MultiPolygon:
                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/polygonLayer.png"));
                break;
            case common::GeometryType::GeometryCollection:
                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/group.png"));
                break;
            default:
                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/unknown.png"));
                break;

            }
        }
        break;
    }

    case common::DatasetType::Raster:
    {
        auto rDs = static_cast<vrsa::raster::RasterDataset*>(dS);
        mMapScene->addRasterDataset(rDs);
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
    }

        if (mTreeWidget)
            mTreeWidget->addTopLevelItem(dSItem);


    emit DatasetAdded(source);

}

void vrsa::services::GISController::ApplyCRS(std::string name)
{  
    if (auto wktDesc = vrsa::georef::SpatialReferenceLibrary::getInstance().GetWKTByName(name))
        mProjCrs = vrsa::gdalwrapper::SpatialReference(*wktDesc, common::CrsFormat::WKT);
    else if (auto projDesc = vrsa::georef::SpatialReferenceLibrary::getInstance().GetProjByName(name))
        mProjCrs = vrsa::gdalwrapper::SpatialReference(*projDesc, common::CrsFormat::Proj);
    else if (auto epsgCode = vrsa::georef::SpatialReferenceLibrary::getInstance().GetEPSGByName(name))
        mProjCrs = vrsa::gdalwrapper::SpatialReference(std::to_string(*epsgCode), common::CrsFormat::EPSG);
    else
    {
        VRSA_ERROR("GISController", "Can't apply CRS:" + name);
        return;
    }
    emit ProjectCRSChanged(mProjCrs);
    //mMapCalculator->setCRS(mProjCrs);
}

bool vrsa::services::GISController::isCurrentCRSGeographic() const
{
    return mProjCrs.GetOGRSpatialRef()->IsGeographic();
}

void vrsa::services::GISController::startDigitizing() const
{
    auto& pM = ProjectManager::instance();
    auto activeLayer = pM.getActiveVectorLayer();
    if (!activeLayer)
        return;
    switch (activeLayer->getGeomType())
    {
    case common::GeometryType::Point:
    case common::GeometryType::MultiPoint:
    {
        auto pointTool = std::make_unique<tools::PointDigitizingTool>(mMapScene);
        connect(pointTool.get(), &tools::PointDigitizingTool::cancelled, mDigitizingManager.get(),
                &DigitizingManager::onDIgitizingCanceled);
        connect(pointTool.get(), &tools::PointDigitizingTool::geometryCreated, mDigitizingManager.get(),
                &DigitizingManager::onGeometryCreated);
        mMapScene->setTool(std::move(pointTool));
        break;
    }
    case common::GeometryType::LineString:
    case common::GeometryType::MultiLineString:
    {
        break;
    }
    default:
    {
        break;
    }
    }

}

void vrsa::services::GISController::showContextMenu(const QPoint &itemPos)
{
    QTreeWidgetItem *clickedItem=mTreeWidget->itemAt(itemPos);
    if (!clickedItem)
        return;
    //получение типа QTreeWidgetItem (dataset, layer etc..)

    QMenu menu;
    switch (clickedItem->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>())
    {
    case common::ItemType_VectorDataset:
        qDebug() << "Vector Dataset Item Clicked!";
        menu.addAction(tr("Свойства"), this, [this, clickedItem]() {
            VectorDatasetForm form;
            form.setTreeItem(clickedItem);
            form.exec();
            qDebug() << "properties";
        });
        break;
    case common::ItemType_RasterDataset:
        qDebug() << "Raster Dataset Item Clicked!";
        menu.addAction(tr("Свойства"), this, [this, clickedItem]() {
//            VectorDatasetForm form;
//            form.setTreeItem(clickedItem);
//            form.exec();
            qDebug() << "properties";
        });
        menu.addAction(tr("Увеличить до слоя"), this, [this, clickedItem]() {

            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            int layerID = clickedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
            auto rasterDataset = static_cast<raster::RasterDataset*>(ProjectManager::instance()
                                                                    .getDatasetBySource(path.toStdString()));

            QRectF targetRect = rasterDataset->getBoundingBox();
            mMapView->fitInView(targetRect, Qt::KeepAspectRatio);
            mMapView->centerOn(targetRect.center());
            mMapView->recalculateScale();
            qDebug() << "zoom to layer";
        });
        break;
    case common::ItemType_VectorLayer:
        qDebug() << "Vector Layer Item Clicked!";

        menu.addAction(tr("Свойства"), this, [this, clickedItem]() {
//            VectorDatasetForm form;
//            form.setTreeItem(clickedItem);
//            form.exec();
            qDebug() << "properties";
        });

        menu.addAction(tr("Увеличить до слоя"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            int layerID = clickedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
            auto selectedLayer = ProjectManager::instance().getLayer(path.toStdString(), layerID);
            QRectF targetRect = selectedLayer->getBoundingBox();
            mMapView->fitInView(targetRect, Qt::KeepAspectRatio);
            mMapView->centerOn(targetRect.center());
            mMapView->recalculateScale();
            qDebug() << "zoom to layer";
        });

        menu.addAction(tr("Таблица атрибутов"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            int layerID = clickedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
            auto selectedLayer = ProjectManager::instance().getLayer(path.toStdString(), layerID);
            AttributeTableForm form(nullptr, selectedLayer);
            //VectorDatasetForm form;
            form.exec();
        });

        break;
    case common::ItemType_RasterLayer:
        break;
    default:
        break;

    }
    menu.show();
    menu.exec(mTreeWidget->mapToGlobal(itemPos));

//    if (clickedItem->parent()) {
//        qDebug() << "Parent found:" << clickedItem->parent()->text(0);
//        QTreeWidgetItem *parentItem = clickedItem->parent();
//        QString parentPath = parentItem->toolTip(0);
//        qDebug() << "Путь родителя:" << parentPath;
//    }

}

void vrsa::services::GISController::layerTreeDataChanged(QTreeWidgetItem *selectedItem, int column)
{
    auto& pM = ProjectManager::instance();
    switch (selectedItem->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>())
    {
    case common::ItemType_VectorDataset:
    {
        QString path = selectedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
        for (auto& layer: static_cast<vector::VectorDataset*>(pM.getDatasetBySource(path.toStdString()))->getLayers())
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
        auto selectedLayer = pM.getLayer(path.toStdString(), id);
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
    auto& pM = ProjectManager::instance();
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
        auto selectedLayer = pM.getLayer(path.toStdString(), id);
        if (selectedLayer)
            pM.setActiveVectorLayer(selectedLayer);
        qDebug()<<pM.getActiveVectorLayer();
        break;
    }
    case common::ItemType_RasterLayer:
        break;
    default:
        break;

    }
}


