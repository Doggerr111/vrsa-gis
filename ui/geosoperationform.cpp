#include "geosoperationform.h"
#include "ui_geosoperationform.h"
#include "vectorlayertools/spatial_operations/spatialoperation.h"
GeosOperationForm::GeosOperationForm(SpatialOperationType type, std::vector<std::string> layerNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeosOperationForm),
    mType{type}
{
    ui->setupUi(this);
    setWindowTitle(vrsa::vector::spatialOperationTypeToQString(mType));
    for (const auto name: layerNames)
        ui->comboBoxFirstInputLayer->addItem(QString::fromStdString(name));
    connect(ui->pushButtonOuputPathBtn, &FileDialogButton::clicked, this, &GeosOperationForm::onFileButtonClicked);

    switch (mType)
    {
    case SpatialOperationType::Buffer:
        ui->frameTolerance->hide();
        ui->frameDistanceMeters->show();
        break;
    case SpatialOperationType::Triangulation:
    case SpatialOperationType::VoronoiDiagramm:
        ui->frameTolerance->show();
        ui->frameDistanceMeters->hide();
        break;
    default:
        break;
    }
    adjustSize();
}

GeosOperationForm::~GeosOperationForm()
{
    delete ui;
}

vrsa::common::SpatialOperationDTO GeosOperationForm::createDTO()
{
    vrsa::common::SpatialOperationDTO dto;
    dto.firstInputLayerName = ui->comboBoxFirstInputLayer->currentText().toStdString();
    dto.outputPath = ui->lineEditOutputPath->text().trimmed().toStdString();
    switch (mType)
    {

    case vrsa::vector::SpatialOperationType::Buffer:
    {

        double distance = ui->doubleSpinBoxDistanceMeters->value();
        dto.distance = distance;
        return dto;
    }
    default:
        return dto;
    }
}

void GeosOperationForm::onFileButtonClicked(QString fileName)
{
    QString ext = "shp";
    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith("." + ext))
        {
            fileName += "." + ext;
        }
    }

    ui->lineEditOutputPath->setText(fileName);
}




void GeosOperationForm::on_pushButtonCancel_clicked()
{
    close();
}


void GeosOperationForm::on_pushButtonAccept_clicked()
{
   emit operationAccepted(createDTO());
}

