#include "attributetableform.h"
#include "ui_attributetableform.h"
#include <QDebug>
AttributeTableForm::AttributeTableForm(QWidget *parent, vectorLayer *vL) :
    QDialog(parent),
    ui(new Ui::AttributeTableForm),
    mLayer{vL}
{
    ui->setupUi(this);
    if (vL!=nullptr)
    {
        setAttributeTable();
    }
}

AttributeTableForm::~AttributeTableForm()
{
    delete ui;
}

void AttributeTableForm::setAttributeTable() const
{
    if (!mLayer)
        return;
    ui->attributeTable->blockSignals(true);
    for (const auto& str: mLayer->getAttributesNames())
        qDebug()<<QString::fromStdString(str);
    auto attributeNames = mLayer->getAttributesNames();
    ui->attributeTable->setColumnCount(attributeNames.size());
    ui->attributeTable->setRowCount(mLayer->getFeaturesCount());

    QStringList headers;
   //headers << "ID";
    for (const auto& name : attributeNames)
    {
        headers << QString::fromStdString(name);
    }
    ui->attributeTable->setHorizontalHeaderLabels(headers);

    auto features = mLayer->getFeatures();
    for (size_t row = 0; row < features.size(); ++row)
    {
        auto* feature = features[row];

//        // Ячейка с ID (не редактируемая)
//        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(row + 1));
//        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
//        idItem->setBackground(QColor(240, 240, 240));
//        idItem->setData(Qt::UserRole, QVariant::fromValue(feature));
//        ui->attributeTable->setItem(static_cast<int>(row), 0, idItem);

        // Ячейки с атрибутами
        for (size_t col = 0; col < attributeNames.size(); ++col)
        {
            std::string attrName = ui->attributeTable->horizontalHeaderItem(col)->text().toStdString();
            //const std::string& attrName = attributeNames[col];
            auto value = feature->getAttribute(attrName);
            //qDebug() << QString::fromStdString(attrName) << " " << attributeValueToQVariant(value).toString();
            auto var = attributeValueToQVariant(value).toString();
            if (var.isEmpty() || var.isNull())
                var = "NULL";
            QTableWidgetItem* item = new QTableWidgetItem(var);

            //сохраняем Feature и имя атрибута
            item->setData(vrsa::common::AttributeTableFeatureRole, QVariant::fromValue(feature));
            item->setData(vrsa::common::AttributeTableFeatureNameRole, QString::fromStdString(attrName));

//            // Настройка внешнего вида
//            customizeItemAppearance(item, value);

            ui->attributeTable->setItem(static_cast<int>(row), static_cast<int>(col), item);
        }
    }

    // Автоподбор ширины столбцов
    ui->attributeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->attributeTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->attributeTable->blockSignals(false);

}

QVariant AttributeTableForm::attributeValueToQVariant(const vrsa::vector::VectorFeature::AttributeValue &value) const
{
    return std::visit([](auto&& arg) -> QVariant {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return QVariant("NULL"); // или QVariant()
        }
        else if constexpr (std::is_same_v<T, int>) {
            return QVariant(arg);
        }
        else if constexpr (std::is_same_v<T, double>) {
            return QVariant(arg);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return QVariant(QString::fromStdString(arg));
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return QVariant(arg);
        }
        else {
            return QVariant();
        }
    }, value);
}

void AttributeTableForm::on_attributeTable_cellChanged(int row, int column)
{
    QTableWidgetItem* item = ui->attributeTable->item(row, column);
    if (!item) return;
    auto feature = item->data(vrsa::common::AttributeTableFeatureRole).value<vrsa::vector::VectorFeature*>();
    if (!feature) return;
    mLayer->getOGRLayer()->ResetReading();
    if (feature->updateAttribute(ui->attributeTable->horizontalHeaderItem(column)->text().toStdString(),
                             item->text().toStdString()))
        qDebug()<< "успех!!";

    mLayer->getOGRLayer()->SyncToDisk();
}

