#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "vectorstylingform.h"
#include <QButtonGroup>
#include <QSplitter>
#include "vectorlayercreationform.h"
#include "test_utils/vectorlayertest.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mGisController{std::make_unique<vrsa::services::GISController>()}
{
    ui->setupUi(this);
    if (!mGisController)
        return;
    initialize();
    mGisController->setupViewComponents(createViewComponents());
    setupLogger();
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    mMapToolExclusiveGroup = new QButtonGroup(this);
    mMapToolExclusiveGroup->setExclusive(true);
    mMapToolExclusiveGroup->addButton(ui->pushButton_addFeature);
    mMapToolExclusiveGroup->addButton(ui->pushButtonSingleSelection);
    mMapToolExclusiveGroup->addButton(ui->pushButtonRectSelection);
    mMapToolExclusiveGroup->addButton(ui->pushButtonGeometryEdit);
    ui->rightTabWidget->tabBar()->setExpanding(false);

    QHBoxLayout* layoutOld = qobject_cast<QHBoxLayout*>(ui->central_frame->layout());
    //убираем из layout
    layoutOld->removeWidget(ui->left_menu_frame);
    layoutOld->removeWidget(ui->graphicsView);
    layoutOld->removeWidget(ui->right_menu_frame);
    delete ui->central_frame->layout();
    //новый layout
    QHBoxLayout* layout = new QHBoxLayout(ui->central_frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    //извлекаем graphicsView из его фрейма
    ui->graphicsView->setParent(nullptr);
    //cоздаем ВЕРТИКАЛЬНЫЙ сплиттер для graphicsView + консоль
    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical);
    verticalSplitter->setHandleWidth(1);
    verticalSplitter->setStyleSheet("QSplitter::handle { background: black; }");
    verticalSplitter->setChildrenCollapsible(false);
    verticalSplitter->addWidget(ui->graphicsView);
    verticalSplitter->addWidget(ui->tabWidgetConsoleLog);
    verticalSplitter->setSizes({500, 150});  // graphicsView 500px, консоль 150px
    //горизонтальный сплитер для tab widgets по бокам
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->setStyleSheet("QSplitter::handle { background: transparent; }");
    splitter->setStyleSheet("QSplitter::handle { background: black; }");
    splitter->addWidget(ui->left_menu_frame);
    splitter->addWidget(verticalSplitter);
    splitter->addWidget(ui->right_menu_frame);
    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(1);
    layout->addWidget(splitter);
}

void MainWindow::setupLogger()
{
    auto& logger = vrsa::common::Logger::getInstance();
    //главная консоль (все сообщения)
    logger.setLogWidget(ui->textEditLoggerCommon);
    logger.setCategoryWidget(vrsa::common::LogCategory::GDAL, ui->textEditLoggerTech);
    logger.setCategoryWidget(vrsa::common::LogCategory::VECTOR, ui->textEditLoggerData);
    logger.setCategoryWidget(vrsa::common::LogCategory::UI, ui->textEditLoggerGraphics);
}

vrsa::services::ViewComponents MainWindow::createViewComponents()
{
    vrsa::services::ViewComponents comps;
    comps.mapView = ui->graphicsView;
    comps.addFeatureBtn = ui->pushButton_addFeature;
    comps.singleSelectionBtn = ui->pushButtonSingleSelection;
    comps.rectSeletionBtn = ui->pushButtonRectSelection;
    comps.geometryEditBtn = ui->pushButtonGeometryEdit;
    comps.mapToolsGrp = mMapToolExclusiveGroup;

    comps.coordEdit = ui->lineEditCoordinates;
    comps.scaleEdit = ui->lineEditScale;
    comps.crsCombo = ui->crsComboBox;
    comps.statusBar = ui->statusbar;
    comps.activeLayerLabel = ui->labelActiveLayer;

    comps.leftTab = ui->leftTabWidget;
    comps.mainLegendTree = ui->LayerTree;
    comps.rightTab = ui->rightTabWidget;
    comps.featureSelectionTree = ui->treeWidgetSelection;

    comps.actionOpenLayer = ui->actionLoad_vector_layer;
    comps.actionCreateLayer = ui->actionNewLayer;
    comps.actionCreatePointLayer = ui->actionNew_point_layer;
    comps.actionCreateLineLayer = ui->actionNew_line_layer;
    comps.actionCreatePolygonLayer = ui->actionNew_polygon_layer;

    return comps;
}



void MainWindow::on_actionLoad_vector_layer_triggered()
{ 
}


void MainWindow::on_crsComboBox_currentIndexChanged(int index)
{
}



void MainWindow::on_pushButton_addFeature_clicked()
{
}


void MainWindow::on_pushButtonSingleSelection_clicked(bool checked)
{
}


void MainWindow::on_pushButton_testVectorLayerUtils_clicked()
{
//    auto scene = ui->graphicsView->scene();
//    std::vector<QPainterPath> paths;
//    for (int i=0; i<1000; ++i)
//    {
//        double lat = QRandomGenerator::global()->generateDouble() * 180 - 90;   // -90..90
//        double lon = QRandomGenerator::global()->generateDouble() * 360 - 180;  // -180..180
//        QPainterPath path;
//        path.moveTo(lon, lat);
//        for (int j=0; j<10; j++)
//        {
//            double lat = QRandomGenerator::global()->generateDouble() * 180 - 90;   // -90..90
//            double lon = QRandomGenerator::global()->generateDouble() * 360 - 180;  // -180..180
//            path.lineTo(lon,lat);

//        }
//        paths.push_back(path);

//    }
//    for (int i=0; i<1000; ++i)
//    {
//        //QGraphicsEllipseItem *item = new QGraphicsEllipseItem();
//        QPen pen;
//        pen.setWidthF(0.1);
//        QGraphicsPathItem* it = new QGraphicsPathItem;
//        it->setPen(pen);
//        it->setPath(paths[i]);
//        scene->addItem(it);
//    }

}

