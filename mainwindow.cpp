#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gdal/gdalreader.h"
#include <QDebug>
#include "graphics/featuregraphicsitemfactory.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mGisController{std::make_unique<vrsa::services::GISController>()}

{
    ui->setupUi(this);

    if (mGisController)
        VRSA_DEBUG("Services", "GIS Controller succesfully created");
    mGisController->initializeScene(ui->graphicsView);

    connect(ui->graphicsView, &MapHolder::scaleChanged, this, &MainWindow::updateScaleLineEdit);
    connect(mGisController->getScene(), &vrsa::graphics::MapScene::mouseMoved, this, &MainWindow::updateCoordinatesLineEdit);


//    vrsa::gdalwrapper::GDALReader reader;
//    auto ptr=reader.readDataset("/home/doger/Documents/vrsa/VRSA/tests/data/KostromskayaBoundary.shp");
//    auto ptr2=reader.readDataset("/home/doger/Documents/vrsa/VRSA/tests/data/testMultiLayers.gpx");
//    auto ptr3=reader.readDataset("/home/doger/Documents/vrsa/VRSA/tests/data/place_points_osm.shp");


//    if (ptr != nullptr)
//    {
//        VRSA_DEBUG("GDAL", "dataset not null!");
//    }
//    else VRSA_DEBUG("GDAL", "dataset null!");
//    //auto l = reader.readLayers(ptr);
//    //qDebug()<<QString::number(l.at(0)->id());
//    //VRSA_DEBUG("GDAL", "FeatureCount: " + std::to_string(ptr2->getLayer(0).featuresCount()));
//    std::vector<std::unique_ptr<vrsa::graphics::FeatureGraphicsItem>> vec;
//    auto scene = new QGraphicsScene();
//    ui->graphicsView->setScene(scene);
//    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
//    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
//    ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
//    double xMin = -20037508.34;
//    double yMax = 20037508.34;
//    double width = 40075016.68;
//    double height = 20037508.34;

//    // Установка сцены в пределах EPSG:3857 координат
//    ui->graphicsView->scale(1,-1);
//    ui->graphicsView->setSceneRect(xMin*4, 4*yMax, 4*width, -8*height);
////    for (int i=0; i<ptr3->getLayer(0).featuresCount(); ++i)
////    {

////        scene->addItem(&*vrsa::graphics::
////                       FeatureGraphicsItemFactory::createForFeature(ptr3->getLayer(0).getFeatureAt(i),
////                      vrsa::graphics::VectorFeatureStyle::createDefaultVectorStyle(vrsa::common::GeometryType::Point)));
////        //VRSA_DEBUG("FeatureGraphicsItemFactory", "Create Feature for place_points_osm.shp #" + std::to_string(i));
////    }

    //sleep(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionLoad_vector_layer_triggered()
{
    std::string fileName=QFileDialog::getOpenFileName(this,"","").toStdString();
    mGisController->LoadDataSet(fileName);

}


void MainWindow::on_crsComboBox_currentIndexChanged(int index)
{
    mGisController->ApplyCRS(ui->crsComboBox->currentText().toStdString());
}

void MainWindow::updateScaleLineEdit(int s)
{
    ui->lineEditScale->setText(QString::number(s));
}

void MainWindow::updateCoordinatesLineEdit(QPointF p)
{
    if (mGisController->isCurrentCRSGeographic())
    {
        QString xCoord = QString::number(p.x(), 'f', 5); // Форматируем координату x с двумя знаками после запятой
        QString yCoord = QString::number(p.y(), 'f', 5); // Форматируем координату y с двумя знаками после запятой
        QString coords = QString("%1  %2").arg(xCoord, yCoord); // Соединяем координаты в одну строку с пробелом между ними
        ui->lineEditCoordinates->setText(coords);
    }
    else
    {
        QString xCoord = QString::number(p.x(), 'f', 2);
        QString yCoord = QString::number(p.y(), 'f', 2);
        QString coords = QString("%1  %2").arg(xCoord, yCoord);
        ui->lineEditCoordinates->setText(coords);
    }
}

