#include "vectorreprojectionform.h"
#include "ui_vectorreprojectionform.h"
#include <QMessageBox>
VectorReprojectionForm::VectorReprojectionForm(std::vector<std::string> layerNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VectorReprojectionForm)
{
    ui->setupUi(this);
    for (const auto name: layerNames)
        ui->comboBoxInputLayer->addItem(QString::fromStdString(name));
    connect(ui->pushButtonOuputPathBtn, &FileDialogButton::clicked, this, &VectorReprojectionForm::onFileButtonClicked);
}

VectorReprojectionForm::~VectorReprojectionForm()
{
    delete ui;
}

void VectorReprojectionForm::on_pushButtonAccept_clicked()
{
    auto path = ui->lineEditOutputPath->text().trimmed();
    if (path.isEmpty())
    {
        QMessageBox::warning(this, tr("Ошибка"), tr("Выберите путь для сохранения перепроецированного слоя"));
        return;
    }
    emit reprojectingAccepted(ui->comboBoxInputLayer->currentText().toStdString(),
                              ui->comboBoxDestinationCRS->currentText().toStdString(),
                              path.toStdString());
    close();
}


void VectorReprojectionForm::on_pushButtonCancel_clicked()
{
    close();
}

void VectorReprojectionForm::onFileButtonClicked(QString fileName)
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

