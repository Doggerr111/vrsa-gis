#include "giscontroller.h"
vrsa::services::GISController::GISController(QObject *parent)
    : QObject{parent},
      mProjManager{},
      mMapView{nullptr}
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
    //connect(ui->graphicsView,SIGNAL(MapHolderResized()),this,SLOT(recalculateScale()));
    connect(mMapView, &MapHolder::scaleChanged, mMapScene, &graphics::MapScene::onMapHolderScaleChanged);


//    connect(this, SIGNAL(newVectorLayer(LIPVectorLayer*)), mMapScene, SLOT(drawVectorLayer(LIPVectorLayer*)));
//    connect(mMapScene,SIGNAL(pos_changed(QPointF)),this,SLOT(scenePos(QPointF)));
//    connect(mMapScene,SIGNAL(scene_dragging(QPointF,QPointF)),this,SLOT(changeExtent(QPointF,QPointF)));

    //connect(scene,SIGNAL(startAdding()),ui->graphicsView,SLOT(onAddingFeatures()));
    //connect(scene,SIGNAL(stopAdding()),ui->graphicsView,SLOT(onStopAddingFeatures()));

    //connect(view, SIGNAL(MapHolderZoomed(double)), this, SLOT(redrawNeeded(double)));


    mMapView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    mMapView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //ui->graphicsView->setAttribute(Qt::WA_OpaquePaintEvent);
    //ui->graphicsView->setAttribute(Qt::WA_NoSystemBackground);
    mMapView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    // Отключаем ненужные функции вида
    view->setOptimizationFlags(
        QGraphicsView::DontSavePainterState |
        QGraphicsView::DontAdjustForAntialiasing
    );

    // Или все оптимизации сразу
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
    //ui->graphicsView->scene()->setSceneRect(xMin, yMax, width, height);

    mMapView->setSceneRect(xMin*4, 4*yMax, 4*width, -8*height);
    mMapView->recalculateScale();
    //ui->graphicsView->setFixedWidth(100);
}

void vrsa::services::GISController::setTreeWidget(TreeWidget *treeWidget)
{
    mTreeWidget = treeWidget;
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mTreeWidget, &TreeWidget::customContextMenuRequested,
           this, &services::GISController::showContextMenu);
//    connect(mTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
//            this, SLOT(showContextMenu(const QPoint&)));
}

void vrsa::services::GISController::LoadDataSet(std::string &source)
{
    vrsa::gdalwrapper::GDALReader reader;
    auto ptr=reader.readDataset(source);
    if (!ptr)
        VRSA_ERROR("GDAL", "Can't read dataset: " + source);
    mProjManager.AddDataset(std::move(ptr));
    //mProjManager->getDatasetBySource(source);
    if (mProjManager.getDatasetBySource(source)->GetDatasetType()==common::DatasetType::Vector)
    {
        auto vDs = static_cast<vrsa::vector::VectorDataset*>(mProjManager.getDatasetBySource(source));
        auto* dSItem = new QTreeWidgetItem();

        dSItem->setIcon(0, QIcon(":/images/icons/dataset.png"));
        dSItem->setText(0, QString::fromStdString(source));
        dSItem->setToolTip(0, QString::fromStdString(source));
        dSItem->setData(0, common::DatasetPathRole, QString::fromStdString(source));
        dSItem->setData(0, common::DatasetTypeRole, QVariant::fromValue(common::DatasetType::Vector));

        for (int i=0; i<vDs->layersCount(); ++i)
        {
            auto& layer = vDs->getLayer(i);
            mMapScene->addLayer(layer);
            auto* layerItem = new QTreeWidgetItem(dSItem);

            layerItem->setData(0, common::DatasetPathRole, QString::fromStdString(source));
            layerItem->setData(0, common::LayerIDRole, i);
            layerItem->setData(0, common::LayerTypeRole, QVariant::fromValue(layer->getGeomType()));

            const char* layerName = layer->getName();
            const char* displayName = "unknown";
            if (layerName && layerName[0] != '\0')
                displayName = layerName;
            layerItem->setText(0, QString::fromUtf8(displayName));
            switch (layer->getGeomType())
            {
            case common::GeometryType::Point:
            case common::GeometryType::MultiPoint:
                layerItem->setIcon(0, QIcon(":/images/icons/pointLayer.png"));
                break;
            case common::GeometryType::LineString:
            case common::GeometryType::MultiLineString:
                layerItem->setIcon(0, QIcon(":/images/icons/lineLayer.png"));
                break;
            case common::GeometryType::Polygon:
            case common::GeometryType::MultiPolygon:
                layerItem->setIcon(0, QIcon(":/images/icons/polygonLayer.png"));
                break;
            case common::GeometryType::GeometryCollection:
                layerItem->setIcon(0, QIcon(":/images/icons/group.png"));
                break;
            default:
                layerItem->setIcon(0, QIcon(":/images/icons/unknown.png"));
                break;


            }
            //layerItem->setData(0, Qt::UserRole,"Layer #" + QString::number(i));
        }
        if (mTreeWidget)
            mTreeWidget->addTopLevelItem(dSItem);
    }
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

void vrsa::services::GISController::showContextMenu(const QPoint &itemPos)
{
    QTreeWidgetItem *clickedItem=mTreeWidget->itemAt(itemPos);
    if (!clickedItem)
        return;
    if (clickedItem->parent()) {
        qDebug() << "Есть родитель! Текст родителя:" << clickedItem->parent()->text(0);

        // Получить родителя
        QTreeWidgetItem *parentItem = clickedItem->parent();
        QString parentPath = parentItem->toolTip(0);
        qDebug() << "Путь родителя:" << parentPath;
    }

}

//void vrsa::services::GISController::showContextMenu(const QPoint &itemPos)
//{
//    QTreeWidgetItem *clickedItem=mTreeWidget->itemAt(itemPos);
//    qDebug()<<"ok1";
//    if (clickedItem==nullptr)
//        return;
//    QString path=clickedItem->toolTip(0);
//    //if (path.isEmpty())
//        //return;
//    qDebug()<<"ok2";

//    if (dynamic_cast<LIPTreeWidgetRasterItem*>(clickedItem)!=nullptr) //если выбран растровый слой
//    {
//        LIPRasterLayer* selectedRasterLayer=LIPProject::getInstance().getRasterLayerByPath(path);
//        if (selectedRasterLayer==nullptr)
//            return;
//        QMenu menu;
//        QAction* actionProperties = new QAction(QString::fromUtf8("Свойства"), this);
//        menu.addAction(actionProperties);
//        menu.addSeparator();
//        QAction* actionStyle = new QAction(QString::fromUtf8("Стиль"), this);
//        menu.addAction(actionStyle);
//        // стилизация
//        connect(actionStyle, &QAction::triggered, this, [selectedRasterLayer]() {
//            LIPRasterStyleDialog* form = new LIPRasterStyleDialog(selectedRasterLayer);
//            form->exec();
//        });
//        menu.addAction(actionStyle);
//        QAction* actionDelete = new QAction(QString::fromUtf8("Удалить"), this);
//        connect(actionDelete, &QAction::triggered, this, [selectedRasterLayer, clickedItem, this]() {

//            for(int i=0; i<LIPProject::getInstance().getRasterLayers().size();i++)
//            {
//                if (LIPProject::getInstance().getRasterLayers().at(i)==selectedRasterLayer)
//                {
//                    LIPProject::getInstance().deleteRasterLayerByPath(selectedRasterLayer->getFileName());

//                    QTreeWidgetItem *parent = clickedItem->parent();
//                    int index;
//                    if (parent) {
//                        index = parent->indexOfChild(clickedItem);
//                        delete parent->takeChild(index);
//                    }
//                    else {
//                        index = ui->LayerTree->indexOfTopLevelItem(clickedItem);
//                        ui->LayerTree->takeTopLevelItem(index);
//                    }
//                    //delete clickedItem;
//                }
//            }
//        });
//        menu.addAction(actionDelete);
//        menu.show();
//        menu.exec(ui->LayerTree->mapToGlobal(f));
//        delete actionProperties;
//        delete actionStyle;
//        delete actionDelete;

//    }
//    qDebug()<<"ok3";
//    LIPXYZConnection* connection = LIPProject::getInstance().getXYZConnectionByConnectionName(path);
//    if (connection)
//    {
//        QMenu menu;
//        QAction* actionDelete = new QAction(QString::fromUtf8("Удалить"), this);
//        connect(actionDelete, &QAction::triggered, this, [path, clickedItem, this]()
//        {
//            LIPProject::getInstance().deleteXYZConnectionByConnectionName(path);

//            QTreeWidgetItem *parent = clickedItem->parent();
//            int index;
//            if (parent) {
//                index = parent->indexOfChild(clickedItem);
//                delete parent->takeChild(index);
//            }
//            else {
//                index = ui->LayerTree->indexOfTopLevelItem(clickedItem);
//                ui->LayerTree->takeTopLevelItem(index);
//            }
//            //delete clickedItem;


//        });
//        menu.addAction(actionDelete);
//        menu.show();
//        menu.exec(ui->LayerTree->mapToGlobal(f));
//        delete actionDelete;
//    }
//    qDebug()<<"ok4";
//    LIPVectorLayer* selectedLayer=LIPProject::getInstance().getVectorLayerByPath(path);
//    if (selectedLayer==nullptr)
//        return;
//    QMenu menu;
//    QAction* actionProperties = new QAction(QString::fromUtf8("Свойства"), this);
//    connect(actionProperties, &QAction::triggered, this, [selectedLayer]() {
//        LIPVectorPropertiesForm* form = new LIPVectorPropertiesForm(selectedLayer);
//        form->exec();
//        delete form;
//    });
//    menu.addAction(actionProperties);
//    menu.addSeparator();
//    QAction* actionStyle = new QAction(QString::fromUtf8("Стиль"), this);
//    menu.addAction(actionStyle);
//    connect(actionStyle, &QAction::triggered, this, [selectedLayer]() {
//        LIPVectorStyleForm* form = new LIPVectorStyleForm(nullptr,selectedLayer);
//        form->exec();
//        delete form;
//    });
//    menu.addAction(actionStyle);
//    QAction* actionDelete = new QAction(QString::fromUtf8("Удалить"), this);
//    connect(actionDelete, &QAction::triggered, this, [selectedLayer, clickedItem, this]() {
//        for(int i=0; i<LIPProject::getInstance().getVectorLayers().size();i++)
//        {
//            if (LIPProject::getInstance().getVectorLayers().at(i)==selectedLayer)
//            {
//                LIPProject::getInstance().deleteVectorByPath(selectedLayer->returnFileName());

//                QTreeWidgetItem *parent = clickedItem->parent();
//                int index;
//                if (parent) {
//                    index = parent->indexOfChild(clickedItem);
//                    delete parent->takeChild(index);
//                }
//                else {
//                    index = ui->LayerTree->indexOfTopLevelItem(clickedItem);
//                    ui->LayerTree->takeTopLevelItem(index);
//                }
//                //delete clickedItem;
//            }
//        }
//    });
//    qDebug()<<"ok5";
//    QAction* attrTable = new QAction(QString::fromUtf8("Таблица атрибутов"), this);
//    connect(attrTable, &QAction::triggered, this, [selectedLayer, clickedItem, this]() {
//        LIPAttributesTableForm* form = new LIPAttributesTableForm();
//        if (selectedLayer!=nullptr)
//        {
//            form->setLayer(selectedLayer);
//            form->exec();

//        }
//        delete form;


//    });
//    menu.addAction(actionDelete);
//    menu.addAction(attrTable);
//    menu.show();
//    menu.exec(ui->LayerTree->mapToGlobal(f));
//    delete actionProperties;
//    delete actionStyle;
//    delete actionDelete;



//}

