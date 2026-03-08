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
    mGisController->setStatusBar(ui->statusbar);
    connect(ui->graphicsView, &MapHolder::scaleChanged, this, &MainWindow::updateScaleLineEdit);
    connect(mGisController->getScene(), &vrsa::graphics::MapScene::mouseMoved, this, &MainWindow::updateCoordinatesLineEdit);
    connect(mGisController.get(), qOverload<const QIcon&>(&vrsa::services::GISController::activeLayerChanged), this,
            qOverload<const QIcon&>(&MainWindow::onActiveLayerChanged));
    connect(mGisController.get(), qOverload<const QString&>(&vrsa::services::GISController::activeLayerChanged), this,
            qOverload<const QString&>(&MainWindow::onActiveLayerChanged));

    connect(ui->pushButtonSingleSelection, &QPushButton::clicked, mGisController.get(),
            &vrsa::services::GISController::onSingleSelectionToolClicked);

    connect(ui->pushButtonRectSelection, &QPushButton::clicked, mGisController.get(),
            &vrsa::services::GISController::onRectSelectionToolClicked);

    connect(ui->pushButtonGeometryEdit, &QPushButton::clicked, mGisController.get(),
            &vrsa::services::GISController::onGeometryEditToolClicked);


    QButtonGroup* toolGroup = new QButtonGroup(this);
    toolGroup->setExclusive(true);
    toolGroup->addButton(ui->pushButton_addFeature);
    toolGroup->addButton(ui->pushButtonSingleSelection);
     toolGroup->addButton(ui->pushButtonRectSelection);

    ui->rightTabWidget->tabBar()->setExpanding(false);


    showMaximized();

    QHBoxLayout* layoutold = qobject_cast<QHBoxLayout*>(ui->central_frame->layout());

    //убираем из layout
    layoutold->removeWidget(ui->left_menu_frame);
    layoutold->removeWidget(ui->graphicsView);
    layoutold->removeWidget(ui->right_menu_frame);

    delete ui->central_frame->layout();

    // Создаем новый layout
    QHBoxLayout* layout = new QHBoxLayout(ui->central_frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Создаем сплиттер
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->setStyleSheet("QSplitter::handle { background: transparent; }");
    splitter->setStyleSheet("QSplitter::handle { background: black; }");
    splitter->addWidget(ui->left_menu_frame);
    splitter->addWidget(ui->graphicsView);
    splitter->addWidget(ui->right_menu_frame);
    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(1);

    // Добавляем сплиттер в layout
    layout->addWidget(splitter);








    // и закидываем в сплиттер
//    QSplitter* splitter = new QSplitter(Qt::Horizontal, ui->central_frame);
//    splitter->setStyleSheet("QSplitter::handle { background: transparent; }");
//    splitter->setHandleWidth(1);  // толщина разделителя
//    splitter->setStyleSheet("QSplitter::handle { background: black; }");  // чтобы с рамкой совпадало
//    splitter->addWidget(ui->left_menu_frame);
//    splitter->addWidget(ui->graphicsView);
//    splitter->addWidget(ui->right_menu_frame);
//    splitter->setChildrenCollapsible(false);
//    splitter->setFrameShape(QFrame::NoFrame);


//    //splitter->setSizes({200, 800, 200});


//    qDebug() << "central_frame margins:"
//             << ui->central_frame->contentsMargins();
//    qDebug() << "layout spacing:" << layout->spacing();
//    qDebug() << "layout margins:" << layout->contentsMargins();


//    splitter->setStyleSheet(
//        "QSplitter::handle {"
//        "   background: red;"
//        "   height: 10px;"  // для вертикального
//        "   width: 10px;"   // для горизонтального
//        "}"
//        "QSplitter {"
//        "   border: 3px solid blue;"
//        "}"
//    );

//    //layout->setStretchFactor(splitter, 1);
//    layout->setAlignment(Qt::Alignment());  // сброс
//    layout->setContentsMargins(0, 0, 0, 0);  // убрать отступы
//    layout->setMargin(0);
//    layout->setSpacing(0);
//    splitter->setContentsMargins(0, 0, 0, 0);

//    ui->centralwidget->layout()->setMargin(0);
//    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 0);

//    layout->setSpacing(0);
//    splitter->setContentsMargins(0, 0, 0, 0);
//    ui->central_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //layout->addStretch(0);  // пружина справа
    //layout->addWidget(splitter);
   // splitter->setGeometry(0, 0, ui->central_frame->width(), ui->central_frame->height());


    //layout->addStretch(0);  // пружина справа
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

