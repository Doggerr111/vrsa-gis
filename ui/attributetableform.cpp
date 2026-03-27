#include "attributetableform.h"
#include "ui_attributetableform.h"
#include <QDebug>
#include "vector/vectorlayer.h"
#include "GisDefines.h"
#include "QMessageBox"
#include <QPushButton>


AttributeTableForm::AttributeTableForm(VectorLayer *vectorLayer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AttributeTableForm),
    mLayer{vectorLayer}
{
    ui->setupUi(this);
    setWindowTitle(tr("Таблица аттрибутов"));
    if (mLayer!=nullptr)
    {
        QString textWithName = ui->labelLayerName->text();
        textWithName+=mLayer->getName();
        ui->labelLayerName->setText(textWithName);
        setAttributeTable();
    }
}

AttributeTableForm::~AttributeTableForm()
{
    mLayer->deselectAllFeatures();
    delete ui;
}

void AttributeTableForm::setAttributeTable() const
{
    ui->attributeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->attributeTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->attributeTable->blockSignals(true);
    for (const auto& str: mLayer->getFieldNames())
        qDebug()<<QString::fromStdString(str);
    auto attributeNames = mLayer->getFieldNames();
    ui->attributeTable->setColumnCount(attributeNames.size());
    //!!!!!!!!!!!!!!!!!//
    ui->attributeTable->horizontalHeader()->setStretchLastSection(true);
    ui->attributeTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->attributeTable->setRowCount(mLayer->getFeaturesCount());
    QStringList headers;
    for (const auto& name : attributeNames)
        headers << QString::fromStdString(name);
    ui->attributeTable->setHorizontalHeaderLabels(headers);
    auto features = mLayer->getFeatures();
    for (size_t row = 0; row < features.size(); ++row)
    {
        auto* feature = features[row];
        for (size_t col = 0; col < attributeNames.size(); ++col)
        {
            std::string attrName = ui->attributeTable->horizontalHeaderItem(col)->text().toStdString();
            auto value = feature->getAttributeAsQVariant(attrName);
            auto var = value.toString();
            if (var.isEmpty() || var.isNull())
                var = "NULL";
            QTableWidgetItem* item = new QTableWidgetItem(var);
            //сохраняем Feature и имя атрибута
            item->setData(vrsa::common::AttributeTableFeatureRole, QVariant::fromValue(feature));
            item->setData(vrsa::common::AttributeTableFeatureNameRole, QString::fromStdString(attrName));
            ui->attributeTable->setItem(static_cast<int>(row), static_cast<int>(col), item);
        }
    }
    ui->attributeTable->blockSignals(false);
}



void AttributeTableForm::on_attributeTable_cellChanged(int row, int column)
{
    QTableWidgetItem* item = ui->attributeTable->item(row, column);
    if (!item) return;
    auto feature = item->data(vrsa::common::AttributeTableFeatureRole).value<vrsa::vector::VectorFeature*>();
    if (!feature) return;
    std::string attrName = item->data(vrsa::common::AttributeTableFeatureNameRole).toString().toStdString();
    QString newValue = item->text();
    qDebug()<< feature->setAttribute(attrName, newValue);
}


void AttributeTableForm::on_attributeTable_itemSelectionChanged()
{
    mLayer->deselectAllFeatures();
    QList<QTableWidgetItem*> selectedItems = ui->attributeTable->selectedItems();
    for (const auto* item: selectedItems)
    {
        auto feature = item->data(vrsa::common::AttributeTableFeatureRole).value<vrsa::vector::VectorFeature*>();
        if (!feature) return;
        feature->setSelected(true);
    }
}


void AttributeTableForm::on_toolButtonSelectAll_clicked()
{
    ui->attributeTable->selectAll();
}


void AttributeTableForm::on_toolButtonDeselectAll_clicked()
{
    ui->attributeTable->clearSelection();
}


void AttributeTableForm::on_toolButtonInvertSelection_clicked()
{
    QItemSelectionModel* selectionModel = ui->attributeTable->selectionModel();
    QModelIndexList selected = selectionModel->selectedIndexes();
    selectionModel->clearSelection();
    for (int row = 0; row < ui->attributeTable->rowCount(); ++row)
    {
        for (int col = 0; col < ui->attributeTable->columnCount(); ++col)
        {
            QModelIndex index = ui->attributeTable->model()->index(row, col);
            if (!selected.contains(index))
                selectionModel->select(index, QItemSelectionModel::Select);
        }
    }
}

QString pluralize(int count, const QString& form1, const QString& form2, const QString& form5)
{
    int n = abs(count) % 100;
    if (n > 10 && n < 20) return form5;
    n %= 10;
    if (n == 1) return form1;
    if (n > 1 && n < 5) return form2;
    return form5;
}



void AttributeTableForm::on_toolButtonDeleteSelectedFeatures_clicked()
{
    QList<QTableWidgetItem*> selectedItems = ui->attributeTable->selectedItems();
    std::vector<vrsa::vector::VectorFeature*> featuresToDelete;
    for (const auto* item: selectedItems)
    {
        auto feature = item->data(vrsa::common::AttributeTableFeatureRole).value<vrsa::vector::VectorFeature*>();
        if (!feature) return;
        featuresToDelete.push_back(feature);
    }
    QString text = tr("Удалить ") + QString::number(featuresToDelete.size()) + " " +
                   pluralize(featuresToDelete.size(),
                             tr("выбранный объект"),
                             tr("выбранных объекта"),
                             tr("выбранных объектов")) + "?";
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Удаление объектов"));
    msgBox.setText(text);
    msgBox.setIcon(QMessageBox::NoIcon);
    QPushButton* yesButton = msgBox.addButton(tr("Да"), QMessageBox::YesRole);
    QPushButton* noButton = msgBox.addButton(tr("Нет"), QMessageBox::NoRole);
    msgBox.exec();
    if (msgBox.clickedButton() == yesButton)
    {
        for (const auto& feature: featuresToDelete)
            mLayer->deleteFeature(feature);
    }
}

