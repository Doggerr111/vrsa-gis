#include "vectorlayercreationform.h"
#include "ui_vectorlayercreationform.h"
#include <QDebug>
#include "attributefieldtypecombobox.h"
#include <QMessageBox>
#include "common/GisDefines.h"
VectorLayerCreationForm::VectorLayerCreationForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VectorLayerCreationForm),
    mGeomType{static_cast<int>(GeometryType::Unknown)}
{
    ui->setupUi(this);
    connect(ui->buttonSaveLayer, &FileDialogButton::clicked, this, &VectorLayerCreationForm::onFileButtonClicked);
    setWindowTitle(tr("Создание векторного слоя"));
    ui->comboBoxLayerFormat->addItem("ESRI Shapefile", "shp");
    ui->comboBoxLayerFormat->addItem("GeoJSON", "geojson");
    ui->comboBoxLayerFormat->addItem("GeoPackage", "gpkg");
    ui->frameGeometry->show();
    setupTableWidget();

}

VectorLayerCreationForm::VectorLayerCreationForm(vrsa::common::GeometryType geomType, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::VectorLayerCreationForm)
{
    ui->setupUi(this);
    ui->frameGeometry->hide();
    mGeomType = static_cast<int>(geomType);
    switch (static_cast<vrsa::common::GeometryType>(mGeomType))
    {
    case GeometryType::Point:
        setWindowTitle(tr("Создание векторного слоя"));
        break;
    case GeometryType::LineString:
        setWindowTitle(tr("Создание линейного векторного слоя"));
        break;
    case GeometryType::Polygon:
        setWindowTitle(tr("Создание полигонального векторного слоя"));
        break;
    default:
        setWindowTitle(tr("Создание векторного слоя"));
        break;
    }
    ui->comboBoxLayerFormat->addItem("ESRI Shapefile", "shp");
    ui->comboBoxLayerFormat->addItem("GeoJSON", "geojson");
    ui->comboBoxLayerFormat->addItem("GeoPackage", "gpkg");
    connect(ui->buttonSaveLayer, &FileDialogButton::clicked, this, &VectorLayerCreationForm::onFileButtonClicked);
    setupTableWidget();
}

VectorLayerCreationForm::~VectorLayerCreationForm()
{
    delete ui;
}


void VectorLayerCreationForm::setupTableWidget()
{
    QStringList headers;
    headers << "№ Атрибута" << "Название поля" << "Тип поля";
    ui->tableWidgetAttributes->setColumnCount(3);
    ui->tableWidgetAttributes->setHorizontalHeaderLabels(headers);
    ui->tableWidgetAttributes->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidgetAttributes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidgetAttributes->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    /* ДОБАВИТЬ ЭТО ВЕЗДЕ!!!!!!1*/
    //дополнительно убираем синий фокус через палитру!!!
    QPalette pal = ui->tableWidgetAttributes->palette();
    pal.setColor(QPalette::Highlight, QColor(64, 64, 64));      //темно-серый выделение
    pal.setColor(QPalette::HighlightedText, Qt::white);         //белый текст при выделении
    ui->tableWidgetAttributes->setPalette(pal);

    ui->tableWidgetAttributes->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void VectorLayerCreationForm::updateRowNumbers()
{
    for (int i = 0; i < ui->tableWidgetAttributes->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->tableWidgetAttributes->item(i, 0);
        item->setText(QString::number(i + 1));
    }
}

vrsa::common::LayerDefinition VectorLayerCreationForm::createLayerDefinition()
{
    using namespace vrsa::common;
    LayerDefinition layerDef;
    layerDef.format = ui->comboBoxLayerFormat->currentText().toStdString();
    layerDef.filePath = ui->lineEditFileName->text().toStdString();
    layerDef.crs = ui->comboBoxCRS->currentText().toStdString();
    if (!ui->textEdit->toPlainText().trimmed().isEmpty())
        layerDef.gdalOptions = parseGdalOptions();
    if (ui->tableWidgetAttributes->rowCount() > 0)
        layerDef.fields = createFieldDefenition();
    layerDef.geometryType = mGeomType;
    return layerDef;

}

std::vector<vrsa::common::FieldDefinition> VectorLayerCreationForm::createFieldDefenition()
{
    using namespace vrsa::common;
    std::vector<FieldDefinition> fieldsDef;
    for (int i=0; i<ui->tableWidgetAttributes->rowCount(); ++i)
    {
        FieldDefinition def;
        QLineEdit *nameEdit = qobject_cast<QLineEdit*>(ui->tableWidgetAttributes->cellWidget(i, 1));
        if (nameEdit)
            def.name = nameEdit->text().trimmed().toStdString();

        QComboBox *typeBox = qobject_cast<QComboBox*>(ui->tableWidgetAttributes->cellWidget(i, 2));

        if (typeBox)
        {
            def.type = typeBox->currentText().toStdString();
            def.typeIndex = typeBox->currentData().toInt();
            qDebug()<<"field index combo box" << def.typeIndex;
        }
        fieldsDef.push_back(def);
    }
    qDebug()<<"fieldDef list size:" << fieldsDef.size();
    return fieldsDef;

}

std::unordered_map<std::string, std::string> VectorLayerCreationForm::parseGdalOptions()
{
    std::unordered_map<std::string, std::string> options;
    QString text = ui->textEdit->toPlainText();

    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines)
    {
        QString trimmed = line.trimmed();

        if (trimmed.isEmpty() || trimmed.startsWith('#')) {
            continue;
        }

        //разделитель "="
        int eqPos = trimmed.indexOf('=');
        if (eqPos > 0)
        {
            QString key = trimmed.left(eqPos).trimmed();
            QString value = trimmed.mid(eqPos + 1).trimmed();
            //убираем кавычки
            if (value.startsWith('"') && value.endsWith('"'))
                value = value.mid(1, value.length() - 2);
            if (value.startsWith("'") && value.endsWith("'"))
                value = value.mid(1, value.length() - 2);

            options[key.toStdString()] = value.toStdString();
        }
    }

    return options;
}


void VectorLayerCreationForm::onFileButtonClicked(QString fileName)
{
    QString ext = ui->comboBoxLayerFormat->currentData().toString();
    QString filter = QString("%1 files (*.%2)")
            .arg(ui->comboBoxLayerFormat->currentText())
            .arg(ext);


    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith("." + ext))
        {
            fileName += "." + ext;
        }
    }

    ui->lineEditFileName->setText(fileName);

}

void VectorLayerCreationForm::on_toolButtonAddAttribute_clicked()
{
    auto row = ui->tableWidgetAttributes->rowCount();
    ui->tableWidgetAttributes->insertRow(row);
    QTableWidgetItem *item = new QTableWidgetItem(QString::number(row + 1));
    ui->tableWidgetAttributes->setItem(row, 0, item);

    QLineEdit *lineEdit = new QLineEdit(ui->tableWidgetAttributes); // parent явно указываем
    lineEdit->setMaxLength(10);
    lineEdit->setPlaceholderText("до 10 символов");
    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // растягиваться
    lineEdit->setContentsMargins(0, 0, 0, 0); // убираем отступы
    ui->tableWidgetAttributes->setCellWidget(row, 1, lineEdit);

    AttributeFieldTypeComboBox *comboBox = new AttributeFieldTypeComboBox(ui->tableWidgetAttributes);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    comboBox->setContentsMargins(0, 0, 0, 0);
    ui->tableWidgetAttributes->setCellWidget(row, 2, comboBox);
    ui->tableWidgetAttributes->verticalHeader()->setDefaultSectionSize(32);
    ui->tableWidgetAttributes->verticalHeader()->setVisible(false); // скрыть заголовки строк
   // ui->tableWidgetAttributes->resizeColumnsToContents();
    ui->tableWidgetAttributes->resizeRowsToContents();

}


void VectorLayerCreationForm::on_toolButtonRemoveAttribute_clicked()
{
    int currentRow = ui->tableWidgetAttributes->currentRow();
    if (currentRow >= 0)
    {
        ui->tableWidgetAttributes->removeRow(currentRow);
        updateRowNumbers();  // пересчитать номера
    }
}



void VectorLayerCreationForm::on_pushButtonCancel_clicked()
{
    close();
}


void VectorLayerCreationForm::on_pushButtonCreateLayer_clicked()
{
    auto def = createLayerDefinition();
    if (def.isValid())
    {
        qDebug()<<"definition valid!";
        emit layerCreationAccepted(def);
    }
    else
        QMessageBox::warning(this, tr("Ошибка создания слоя"), tr("Ошибка создания слоя: некорректный путь сохранения"));

}



void VectorLayerCreationForm::on_comboBoxGeometry_currentIndexChanged(int index)
{
    mGeomType = ui->comboBoxGeometry->currentData().toInt();
    qDebug()<<static_cast<vrsa::common::GeometryType>(mGeomType);
}

