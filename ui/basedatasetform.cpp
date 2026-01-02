#include "basedatasetform.h"

BaseDatasetForm::BaseDatasetForm(QWidget *parent)
    :QDialog(parent)
{

}

void BaseDatasetForm::loadFromItem(QTreeWidgetItem *item)
{
    if (!item)
        return;
    mFilePath = item->data(0, vrsa::common::DatasetPathRole).value<QString>();
}
