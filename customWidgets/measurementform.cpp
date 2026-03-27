#include "measurementform.h"
#include "ui_measurementform.h"

MeasurementForm::MeasurementForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementForm),
    mStartPos{},
    mEndPos{}
{
    ui->setupUi(this);
    setWindowModality(Qt::NonModal);
    setWindowTitle(tr("Измерение расстояний"));
    setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    ui->frameEllipsoid->hide();
    adjustSize();
    show();
}

void MeasurementForm::setDistance(double distance)
{
    ui->lineEditDistance->setText(QString::number(distance, 'f', 2));
}

bool MeasurementForm::isGeodesicalMode() const
{
    return ui->radioButtonGeodesical->isChecked();
}

bool MeasurementForm::isCartesianMode() const
{
    return ui->radioButtonCartesian->isChecked();
}

std::string MeasurementForm::getEllipsoid() const
{
    return ui->comboBox->currentText().toStdString();
}

MeasurementForm::~MeasurementForm()
{
    delete ui;
}

void MeasurementForm::on_comboBox_currentIndexChanged(int index)
{
    emit ellipsoidChanged(ui->comboBox->currentText().toStdString());
}


void MeasurementForm::on_radioButtonCartesian_clicked()
{
    emit modeChanged();
    ui->frameEllipsoid->hide();
    adjustSize();
}


void MeasurementForm::on_radioButtonGeodesical_clicked()
{
    emit modeChanged();
    ui->frameEllipsoid->show();
    adjustSize();
}


void MeasurementForm::on_pushButtonExit_clicked()
{
    close();
}

