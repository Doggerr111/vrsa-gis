#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "vectorstylingform.h"
#include <QButtonGroup>
#include <QSplitter>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mGisController{std::make_unique<vrsa::services::GISController>()}

{
    ui->setupUi(this);

    if (mGisController)
        VRSA_DEBUG("Services", "GIS Controller succesfully created");
    mGisController->initializeScene(ui->graphicsView);
    mGisController->setTreeWidget(ui->LayerTree);
    mGisController->setTabWidgets(ui->leftTabWidget, ui->rightTabWidget);
    connect(ui->graphicsView, &MapHolder::scaleChanged, this, &MainWindow::updateScaleLineEdit);
    connect(mGisController->getScene(), &vrsa::graphics::MapScene::mouseMoved, this, &MainWindow::updateCoordinatesLineEdit);
    connect(mGisController.get(), qOverload<const QIcon&>(&vrsa::services::GISController::activeLayerChanged), this,
            qOverload<const QIcon&>(&MainWindow::onActiveLayerChanged));
    connect(mGisController.get(), qOverload<const QString&>(&vrsa::services::GISController::activeLayerChanged), this,
            qOverload<const QString&>(&MainWindow::onActiveLayerChanged));

    connect(ui->pushButtonSingleSelection, &QPushButton::clicked, mGisController.get(),
            &vrsa::services::GISController::onSingleSelectionToolClicked);


    QButtonGroup* toolGroup = new QButtonGroup(this);
    toolGroup->setExclusive(true);
    toolGroup->addButton(ui->pushButton_addFeature);
    toolGroup->addButton(ui->pushButtonSingleSelection);

    ui->rightTabWidget->tabBar()->setExpanding(false);

    //ui->left_menu_frame->setAlignment(Qt::AlignLeft);
    //ui->right_menu_frame->setAlignment(Qt::AlignRight);

    //ui->central_frame->layout()->addWidget(new QFrame());

    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui->central_frame->layout());

    //убираем из layout
    layout->removeWidget(ui->left_menu_frame);
    layout->removeWidget(ui->graphicsView);
    layout->removeWidget(ui->right_menu_frame);

    // и закидываем в сплиттер
    QSplitter* splitter = new QSplitter(Qt::Horizontal, ui->central_frame);
    splitter->setStyleSheet("QSplitter::handle { background: transparent; }");
    splitter->setHandleWidth(1);  // толщина разделителя
    splitter->setStyleSheet("QSplitter::handle { background: black; }");  // чтобы с рамкой совпадало
    splitter->addWidget(ui->left_menu_frame);
    splitter->addWidget(ui->graphicsView);
    splitter->addWidget(ui->right_menu_frame);
    splitter->setChildrenCollapsible(false);

    layout->addWidget(splitter);
    //toolGroup->addButton(ui->zoomButton);

//    for (int i=0; i<9; i++)
//    {
//        VectorStylingForm form(i);
//        form.exec();
//    }
//    std::exit(1);



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

void MainWindow::updateScaleLineEdit(int mapScale, double widgetScale)
{
    ui->lineEditScale->setText(QString::number(mapScale));
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

void MainWindow::onActiveLayerChanged(const QIcon &icon)
{
    ui->pushButton_addFeature->setIcon(icon);
}

void MainWindow::onActiveLayerChanged(const QString &name)
{
    ui->labelActiveLayer->setText(tr("Активный слой:") + name);
}


void MainWindow::on_pushButton_addFeature_clicked()
{
    if (ui->pushButton_addFeature->isChecked())
        mGisController->startDigitizing();
    else
        mGisController->stopDigitizing();
}


void MainWindow::on_pushButtonSingleSelection_clicked(bool checked)
{

}

