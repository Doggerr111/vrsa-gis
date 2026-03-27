#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QButtonGroup>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mGisController{std::make_unique<vrsa::services::GISController>()}
{
    ui->setupUi(this);
    setup();
    mGisController->setupViewComponents(createViewComponents());
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    setupWidgets();
    setupActions();
    setupLogger();
    setupStyle();
}

void MainWindow::setupWidgets()
{
    mMapToolExclusiveGroup = new QButtonGroup(this);
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

void MainWindow::setupStyle()
{
    setWindowTitle("VRSA");
    //цвет выделения (фокуса)
    QPalette pal = ui->LayerTree->palette();
    pal.setColor(QPalette::Highlight, QColor(184, 92, 26));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    ui->LayerTree->setPalette(pal);
    ui->DBLayerTree->setPalette(pal);
    ui->treeWidgetSelection->setPalette(pal);
}

void MainWindow::setupActions()
{
    ui->menuPanels->addAction(ui->actionShowConsole);
    ui->menuPanels->addAction(ui->actionShowLeftTabWidget);
    ui->menuPanels->addAction(ui->actionShowRightTabWidget);
    connect(ui->pushButtonPostGis, &QPushButton::clicked, ui->actionConnectToPostGIS, &QAction::trigger);
    connect(ui->pushButtonIntersection, &QPushButton::clicked, ui->actionIntersection, &QAction::trigger);
    connect(ui->pushButtonVoronoi, &QPushButton::clicked, ui->actionVoronoiDiagram, &QAction::trigger);
    connect(ui->pushButtonTriangulation, &QPushButton::clicked, ui->actionGeosTriangulation, &QAction::trigger);
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
    comps.rulerBtn = ui->pushButtonRuler;
    comps.panBtn = ui->pushButtonPanning;
    comps.zoomInBtn = ui->pushButtonZoomIn;
    comps.zoomOutBtn = ui->pushButtonZoomOut;

    comps.coordEdit = ui->lineEditCoordinates;
    comps.scaleEdit = ui->lineEditScale;
    comps.crsCombo = ui->crsComboBox;
    comps.lodBtn = ui->toolButtonLOD;
    comps.statusBar = ui->statusbar;
    comps.activeLayerLabel = ui->labelActiveLayer;

    comps.leftTab = ui->leftTabWidget;
    comps.mainLegendTree = ui->LayerTree;
    comps.rightTab = ui->rightTabWidget;
    comps.featureSelectionTree = ui->treeWidgetSelection;
    comps.postGisTree = ui->DBLayerTree;

    comps.actionOpenLayer = ui->actionLoad_vector_layer;
    comps.actionCreateLayer = ui->actionNewLayer;
    comps.actionCreatePointLayer = ui->actionNew_point_layer;
    comps.actionCreateLineLayer = ui->actionNew_line_layer;
    comps.actionCreatePolygonLayer = ui->actionNew_polygon_layer;

    comps.actionCreateBuffer = ui->actionBuffer;
    comps.actionCreateTriangulation = ui->actionGeosTriangulation;
    comps.actionCreateVoronoiDiag = ui->actionVoronoiDiagram;

    comps.actionCreateIntersection = ui->actionIntersection;
    comps.actionCreateUnion = ui->actionUnion;
    comps.actionCreateDifference = ui->actionDifference;
    comps.actionCreateSymDifference = ui->actionSymDifference;
    comps.actionReprojectVector = ui->actionVectorReproject;

    comps.actionOpenRasterLayer = ui->actionOpenRasterLayer;

    comps.actionReprojectRaster = ui->actionRasterReproject;
    comps.actionCutRasterByVectorMask = ui->actionCutRasterByVectorMask;
    comps.actionCreateIsolines = ui->actionRasterContours;

    comps.actionWMSConnection = ui->actionConnectToWMS;
    comps.actionXYZConnection = ui->actionConnectToXYZ;

    comps.actionPostGisConnection = ui->actionConnectToPostGIS;

    return comps;
}


void MainWindow::on_actionShowConsole_toggled(bool arg1)
{
    ui->tabWidgetConsoleLog->setVisible(arg1);
}


void MainWindow::on_actionShowRightTabWidget_toggled(bool arg1)
{
    ui->right_menu_frame->setVisible(arg1);
}


void MainWindow::on_actionShowLeftTabWidget_toggled(bool arg1)
{
    ui->left_menu_frame->setVisible(arg1);
}


void MainWindow::on_actionShowToolBar_toggled(bool arg1)
{
    ui->header_tool_bar_frame->setVisible(arg1);
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



