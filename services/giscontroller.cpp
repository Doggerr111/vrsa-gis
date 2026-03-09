#include "giscontroller.h"
#include "vectordatasetform.h"
#include "attributetableform.h"
#include "vectorstylingform.h"
#include "tools/digitizingtoolfactory.h"
#include "tools/maptoolfactory.h"
vrsa::services::GISController::GISController(QObject *parent)
    : QObject{parent},
      mMapView{nullptr},
      //mDigitizingManager{std::make_unique<DigitizingManager>()},
      mRenderer{}
{
    connect(this, &GISController::updateLegendIconsRequired, &mRenderer, &graphics::SymbolRenderer::renderToIcon);
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
    mMapScene->setMapHolderScale(mMapView->getMapHolderScale());
    mMapView->setScene(mMapScene);
    connect(mMapView, &MapHolder::scaleChanged, mMapScene, &graphics::MapScene::onMapHolderScaleChanged);
    connect(mMapScene, &graphics::MapScene::panningRequested, mMapView, &MapHolder::onPanningRequested);
    connect(mMapView, &MapHolder::mousePressed, mMapScene, &graphics::MapScene::onMapHolderMousePressed);
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
    treeWidget->setItemsExpandable(false);
    treeWidget->expandAll();
    connect(mTreeWidget, &TreeWidget::customContextMenuRequested,
            this, &services::GISController::showContextMenu);

    connect(mTreeWidget, &TreeWidget::itemChanged,
            this, &services::GISController::layerTreeDataChanged);

    connect(mTreeWidget, &TreeWidget::itemDoubleClicked,
            this, &services::GISController::onLayerTreeItemDoubleClicked);

    connect(mTreeWidget, &TreeWidget::itemDragRequested,
            this, &services::GISController::onItemDragRequested);

    connect(mTreeWidget, &TreeWidget::itemDropped,
            this, &services::GISController::onItemDropped);

}

void vrsa::services::GISController::setTabWidgets(QTabWidget *lhs, QTabWidget *rhs) noexcept
{
    mLeftTabWidget = lhs;
    mRightTabWidget = rhs;
}

QTreeWidget* vrsa::services::GISController::getSelectionTreeWidget() const
{
    if (mRightTabWidget)
    {
        if (QWidget* pageTab = mRightTabWidget->widget(SELECTION_TAB_INDEX))
            return qobject_cast<QTreeWidget*>(pageTab->findChild<QTreeWidget*>(SELECTION_TREE_OBJ_NAME));
    }
    return nullptr;
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
            if (!layer)
                continue;
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
//            switch (layer->getGeomType())
//            {
//            case common::GeometryType::Point:
//            case common::GeometryType::MultiPoint:
//                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/pointLayer.png"));
//                break;
//            case common::GeometryType::LineString:
//            case common::GeometryType::MultiLineString:
//                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/lineLayer.png"));
//                break;
//            case common::GeometryType::Polygon:
//            case common::GeometryType::MultiPolygon:
//                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/polygonLayer.png"));
//                break;
//            case common::GeometryType::GeometryCollection:
//                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/group.png"));
//                break;
//            default:
//                layerItem->setIcon(DATA_COLUMN, QIcon(":/images/icons/unknown.png"));
//                break;

//            }

            qDebug()<<"GIS CONTROLLER. LAYER GEOM TYPE - " << layer->getGeomType();
            if (auto layerStyle = layer->getStyle(layer->getGeomType()))
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
    default:

        VRSA_WARNING("GisController", "Данный тип датасета пока не совсем ... поддерживается, простите :c");
    }

    if (mTreeWidget)
        mTreeWidget->addTopLevelItem(dSItem);

    mTreeWidget->expandAll();
    syncZOrderWithTree();
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



void vrsa::services::GISController::syncZOrderWithTree()
{
    int baseZ = common::MAX_Z_VALUE;
    for (int i=0; i<mTreeWidget->topLevelItemCount(); ++i)
    {
        auto topLevelItem = mTreeWidget->topLevelItem(i);
        auto& pM = ProjectManager::instance();
        switch (topLevelItem->data(DATA_COLUMN, common::ItemTypeRole).value<common::TreeItemType>())
        {
        case common::ItemType_VectorDataset:
        {
            QString path = topLevelItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            for (auto& layer: static_cast<vector::VectorDataset*>(pM.getDatasetBySource(path.toStdString()))->getLayers())
            {
                //qDebug()<<layer.get()<< " has z value =" << baseZ;
                layer->setZValue(baseZ--);

            }
            break;
        }
        case common::ItemType_RasterDataset:
        {
            QString path = topLevelItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            auto rasterDataset = static_cast<raster::RasterDataset*>(pM.getDatasetBySource(path.toStdString()));
            //qDebug()<<rasterDataset<< " has z value =" << baseZ;
            rasterDataset->setZValue(baseZ--);
            break;
        }
        default:
            break;
        }
    }
}

QIcon vrsa::services::GISController::getIconForGeometryType(common::GeometryType type)
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

        menu.addAction(tr("Настройки стиля"), this, [this, clickedItem]() {
            QString path = clickedItem->data(DATA_COLUMN, common::DatasetPathRole).toString();
            int layerID = clickedItem->data(DATA_COLUMN, common::LayerIDRole).toInt();
            auto selectedLayer = ProjectManager::instance().getLayer(path.toStdString(), layerID);
            auto layerGeometryType = selectedLayer->getGeomType();
            auto style = selectedLayer->getStyle(layerGeometryType);
            if (!style)
                return;
            auto symbol = style->getSymbol();
            if (!symbol)
                return;
            VectorStylingForm form(symbol, layerGeometryType);
            form.exec();
            QIcon icon = QIcon();
            emit updateLegendIconsRequired(symbol, icon);
            clickedItem->setIcon(DATA_COLUMN, icon);

            //            switch (selectedLayer->getGeomType())
            //            {
            //            case common::GeometryType::Point:
            //                qDebug()<<"Point Layer";
            //                break;
            //            case common::GeometryType::MultiPoint:
            //                qDebug()<<"MultiPoint Layer";
            //                break;
            //            case common::GeometryType::LineString:
            //                qDebug()<<"Line Layer";
            //                break;
            //            case common::GeometryType::MultiLineString:
            //                qDebug()<<"MultiLine Layer";
            //                break;
            //            case common::GeometryType::Unknown:
            //                qDebug()<<"Unknown Layer";
            //                break;
            //            }
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
        {

            pM.setActiveVectorLayer(selectedLayer);
            qDebug()<< "Active vector layer changed:" << pM.getActiveVectorLayer();
            emit activeLayerChanged(getIconForGeometryType(selectedLayer->getGeomType()));
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

    QTreeWidgetItem* source = mTreeWidget->currentItem();
    QTreeWidgetItem* target = mTreeWidget->itemAt(event->pos());
    if (!source || source == target)
    {
        *accepted = false;
        return;
    }
    auto& pM = ProjectManager::instance();
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
        QRect targetRect = mTreeWidget->visualItemRect(target);
        int cursorY = event->pos().y();

        //получаем toplevelitem target'a
        QTreeWidgetItem* rootTarget = target;
        while (rootTarget->parent())
        {
            rootTarget = rootTarget->parent();
        }
        int sourceIndex = mTreeWidget->indexOfTopLevelItem(source); //всегда dataset должен быть корнем ??
        int rootTargetIndex = mTreeWidget->indexOfTopLevelItem(rootTarget);
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

                QTreeWidgetItem* temp1 = mTreeWidget->takeTopLevelItem(sourceIndex); //убираем то что перемещаем

                qDebug() << "After take source - items:";
                for (int i = 0; i < mTreeWidget->topLevelItemCount(); ++i) {
                    qDebug() << "  " << i << ":" << mTreeWidget->topLevelItem(i)->text(0);
                }
                rootTargetIndex = mTreeWidget->indexOfTopLevelItem(rootTarget);
                QTreeWidgetItem* temp2 = mTreeWidget->takeTopLevelItem(rootTargetIndex);

                qDebug() << "After both takes - items:";
                for (int i = 0; i < mTreeWidget->topLevelItemCount(); ++i) {
                    qDebug() << "  " << i << ":" << mTreeWidget->topLevelItem(i)->text(0);
                }

                mTreeWidget->insertTopLevelItem(rootTargetIndex, temp1);
                mTreeWidget->insertTopLevelItem(rootTargetIndex+1,
                            temp2);

                qDebug() << "After inserts - items:";
                for (int i = 0; i < mTreeWidget->topLevelItemCount(); ++i) {
                    qDebug() << "  " << i << ":" << mTreeWidget->topLevelItem(i)->text(0);
                }

                mTreeWidget->update();  // не всегда помогает
                mTreeWidget->doItemsLayout();  // принудительная перерисовка

                *accepted=false;
            }
            else
            {
                QTreeWidgetItem* temp1 = mTreeWidget->takeTopLevelItem(sourceIndex); //убираем то что перемещаем

                mTreeWidget->insertTopLevelItem(rootTargetIndex+1, temp1);
                *accepted=false;
            }
        }
        else //если перемещаемый стоит выше того на чье место перемещаем
        {
            if (cursorY < targetRect.center().y()) // верхняя половина — вставить перед
            {
                QTreeWidgetItem* temp1 = mTreeWidget->takeTopLevelItem(sourceIndex); //убираем то что перемещаем
                //QTreeWidgetItem* temp2 = mTreeWidget->takeTopLevelItem(rootTargetIndex);
                rootTargetIndex = mTreeWidget->indexOfTopLevelItem(rootTarget);
                mTreeWidget->insertTopLevelItem(rootTargetIndex, temp1);
                //mTreeWidget->insertTopLevelItem(sourceIndex, temp2);
                *accepted=true;
            }
            else
            {
                QTreeWidgetItem* temp1 = mTreeWidget->takeTopLevelItem(sourceIndex); //убираем то что перемещаем
                rootTargetIndex = mTreeWidget->indexOfTopLevelItem(rootTarget);
                mTreeWidget->insertTopLevelItem(rootTargetIndex+1, temp1);
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
    auto& pM = ProjectManager::instance();
    auto activeLayer = pM.getActiveVectorLayer();
    if (!activeLayer)
    {
        //emit activeLayerChanged(getIconForGeometryType(common::GeometryType::Unknown));//for digitizing tool button
        VRSA_DEBUG("GIS Controller", "Please choose active layer before digitizing");
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
        VRSA_DEBUG("GIS Controller", "Digitizing for active layer geometry type currently not supported");
    }



}



void vrsa::services::GISController::addMapTool(common::MapToolType type, vector::VectorLayer* layer)
{
    mMapScene->deselectCurrentMapTool();
    //mDigitizingManager->onDigitizingFinished();
    QMessageBox::warning(nullptr, "we save yet!", "we save yey!!!");
    auto tool = tools::MapToolFactory::createForScene(mMapScene, type, layer);
    if (tool)
    {
        connect(tool.get(), &tools::MapTool::toolEvent, this, &GISController::onToolEvent);
        if (mStatusBar)
            connect(tool.get(), &tools::MapTool::statusMessage, mStatusBar, &QStatusBar::showMessage);
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
    default:
        break;
    }
}

void vrsa::services::GISController::handleFeatureSelected(graphics::FeatureGraphicsItem *item, bool shouldClearTree)
{
    if (shouldClearTree)
        getSelectionTreeWidget()->clear();
    if (!item)
        return;
    auto feature = item->getFeature();
    auto layer = ProjectManager::instance().getLayerAssociatedWithFeature(feature);
    if (!layer)
        return;
    auto selectionTreeWidget = getSelectionTreeWidget(); //получаем tree виджет из tabWidget
    QTreeWidgetItem* root = new QTreeWidgetItem;
    root->setText(DATA_COLUMN, layer->getName());
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
    getSelectionTreeWidget()->clear();
    for (const auto& item: items)
        handleFeatureSelected(item, false);
}


