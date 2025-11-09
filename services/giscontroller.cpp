#include "giscontroller.h"

vrsa::services::GISController::GISController(QObject *parent)
    : QObject{parent},
      mProjManager{std::make_unique<ProjectManager>()}
{

}

void vrsa::services::GISController::initializeScene(QGraphicsView *view)
{
    if (!view)
        VRSA_ERROR("MapScene initialization", "nullptr view");
    auto scene= new vrsa::graphics::MapScene(view);
    view->setScene(scene);
    //connect(ui->graphicsView,SIGNAL(MapHolderResized()),this,SLOT(recalculateScale()));
    connect(this, SIGNAL(newVectorLayer(LIPVectorLayer*)), scene, SLOT(drawVectorLayer(LIPVectorLayer*)));
    connect(scene,SIGNAL(pos_changed(QPointF)),this,SLOT(scenePos(QPointF)));
    connect(scene,SIGNAL(scene_dragging(QPointF,QPointF)),this,SLOT(changeExtent(QPointF,QPointF)));

    //connect(scene,SIGNAL(startAdding()),ui->graphicsView,SLOT(onAddingFeatures()));
    //connect(scene,SIGNAL(stopAdding()),ui->graphicsView,SLOT(onStopAddingFeatures()));

    connect(view, SIGNAL(MapHolderZoomed(double)), this, SLOT(redrawNeeded(double)));
    view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //ui->graphicsView->setAttribute(Qt::WA_OpaquePaintEvent);
    //ui->graphicsView->setAttribute(Qt::WA_NoSystemBackground);
    view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    // ui->graphicsView->scene()->setSceneRect(-180,90,360,-180);
    double xMin = -20037508.34;
    double yMax = 20037508.34;
    double width = 40075016.68;
    double height = 20037508.34;

    // Установка сцены в пределах EPSG:3857 координат
    view->scale(1,-1);
    //ui->graphicsView->scene()->setSceneRect(xMin, yMax, width, height);

    view->setSceneRect(xMin*4, 4*yMax, 4*width, -8*height);
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
