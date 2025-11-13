#include "giscontroller.h"

vrsa::services::GISController::GISController(QObject *parent)
    : QObject{parent},
      mProjManager{std::make_unique<ProjectManager>()},
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
    connect(this, SIGNAL(newVectorLayer(LIPVectorLayer*)), mMapScene, SLOT(drawVectorLayer(LIPVectorLayer*)));
    connect(mMapScene,SIGNAL(pos_changed(QPointF)),this,SLOT(scenePos(QPointF)));
    connect(mMapScene,SIGNAL(scene_dragging(QPointF,QPointF)),this,SLOT(changeExtent(QPointF,QPointF)));

    //connect(scene,SIGNAL(startAdding()),ui->graphicsView,SLOT(onAddingFeatures()));
    //connect(scene,SIGNAL(stopAdding()),ui->graphicsView,SLOT(onStopAddingFeatures()));

    connect(view, SIGNAL(MapHolderZoomed(double)), this, SLOT(redrawNeeded(double)));
    mMapView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    mMapView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //ui->graphicsView->setAttribute(Qt::WA_OpaquePaintEvent);
    //ui->graphicsView->setAttribute(Qt::WA_NoSystemBackground);
    mMapView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    // ui->graphicsView->scene()->setSceneRect(-180,90,360,-180);
    double xMin = -20037508.34;
    double yMax = 20037508.34;
    double width = 40075016.68;
    double height = 20037508.34;

    // Установка сцены в пределах EPSG:3857 координат
    mMapView->scale(1,-1);
    //ui->graphicsView->scene()->setSceneRect(xMin, yMax, width, height);

    mMapView->setSceneRect(xMin*4, 4*yMax, 4*width, -8*height);
    //ui->graphicsView->setFixedWidth(100);
}

void vrsa::services::GISController::LoadDataSet(std::string &source)
{
    vrsa::gdalwrapper::GDALReader reader;
    auto ptr=reader.readDataset(source);
    if (!ptr)
        VRSA_ERROR("GDAL", "Can't read dataset: " + source);
    mProjManager->AddDataset(std::move(ptr));
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

