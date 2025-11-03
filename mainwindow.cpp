#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gdal/gdalvectorreader.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    vrsa::gdalwrapper::GDALVectorReader reader;
    auto ptr=reader.readDataset("/home/doger/Documents/vrsa/VRSA/tests/data/KostromskayaBoundary.shp");
    auto ptr2=reader.readDataset("/home/doger/Documents/vrsa/VRSA/tests/data/testMultiLayers.gpx");
    if (ptr != nullptr)
    {
        VRSA_DEBUG("GDAL", "dataset not null!");
    }
    else VRSA_DEBUG("GDAL", "dataset null!");
    //auto l = reader.readLayers(ptr);
    //qDebug()<<QString::number(l.at(0)->id());
    sleep(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

