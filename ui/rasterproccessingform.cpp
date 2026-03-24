#include "rasterproccessingform.h"
#include "ui_rasterproccessingform.h"

RasterProccessingForm::RasterProccessingForm(vrsa::common::RasterOperationType type,
                                             const std::vector<std::string>& rasterNames,
                                             const std::vector<std::string>& vectorNames,
                                             QWidget *parent)
    : QDialog(parent),
      ui(new Ui::RasterProccessingForm),
      mOperationType(type)
{
    ui->setupUi(this);
    for (const auto& raster: rasterNames)
        ui->comboBoxInputRaster->addItem(QIcon(":/images/icons/rasterTreeItem.png"), QString::fromStdString(raster));
    for (const auto& vector: vectorNames)
        ui->comboBoxSecondInputLayer->addItem(QString::fromStdString(vector));
    setup();
}

RasterProccessingForm::~RasterProccessingForm()
{
    delete ui;
}

vrsa::common::RasterProcessingParams RasterProccessingForm::createParamsDTO()
{
    using namespace vrsa::common;
    RasterProcessingParams params;
    params.inputPath     = ui->comboBoxInputRaster->currentText().toStdString();
    params.cropToCutline = true;
    params.cutlinePath   = ui->comboBoxSecondInputLayer->currentText().toStdString();
    params.outputPath    = ui->lineEditOutputPath->text().trimmed().toStdString();
    params.resample      = ui->comboBoxResampling->currentData().value<ResamplingMethod>();
    params.compression   = ui->comboBoxCompression->currentData().value<GeoTiffCompression>();
    params.targetCRS     = ui->comboBoxTargetCRS->currentText().toStdString();
    params.interval      = ui->doubleSpinBoxContoursInterval->value();
    params.elevationFieldName = ui->lineEditElevationFieldName->text().trimmed().toStdString();
    return params;

}

void RasterProccessingForm::setup()
{
    using namespace vrsa::common;
    ui->pushButtonOuputPathBtn->setRole(FileDialogButton::ButtonRole::SaveRasterDataset);
    connect(ui->pushButtonOuputPathBtn, &FileDialogButton::clicked, this, &RasterProccessingForm::onFileButtonClicked);
    setWindowTitle(rasterOperationTypeToString(mOperationType));
    switch (mOperationType)
    {
    case RasterOperationType::Reproject:
    {
        ui->frameReproject->show();
        ui->frameContours->hide();
        ui->frameCutRaster->hide();
        break;
    }
    case RasterOperationType::CutByVectorMask:
    {
        ui->frameReproject->hide();
        ui->frameContours->hide();
        ui->frameCutRaster->show();
        break;
    }
    case RasterOperationType::Isolines:
    {
        ui->pushButtonOuputPathBtn->setRole(FileDialogButton::ButtonRole::SaveDataset);
        ui->frameReproject->hide();
        ui->frameCutRaster->hide();
        ui->frameCompression->hide();
        ui->frameResampling->hide();
        ui->frameContours->show();
        break;
    }
    default:
        break;
    }
    adjustSize();
}

void RasterProccessingForm::on_pushButtonAccept_clicked()
{
    emit operationAccepted(createParamsDTO());
}


void RasterProccessingForm::on_pushButtonCancel_clicked()
{
    close();
}

void RasterProccessingForm::onFileButtonClicked(QString fileName)
{
    QString ext;
    if (mOperationType != vrsa::common::RasterOperationType::Isolines)
        ext = "tiff";
    else
        ext = "shp";
    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith("." + ext))
        {
            fileName += "." + ext;
        }
    }

    ui->lineEditOutputPath->setText(fileName);
}

