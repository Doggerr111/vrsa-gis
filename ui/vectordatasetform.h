#ifndef VECTORDATASETFORM_H
#define VECTORDATASETFORM_H

#include "basedatasetform.h"
#include "services/projectmanager.h"

namespace Ui {
class VectorDatasetForm;
}

class VectorDatasetForm : public BaseDatasetForm
{
    Q_OBJECT

public:
    explicit VectorDatasetForm(QWidget *parent = nullptr);
    ~VectorDatasetForm();
    void setTreeItem(QTreeWidgetItem* item);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void openFileLocation();


private:
    Ui::VectorDatasetForm *ui;

    // BaseDatasetForm interface
//public:
//    void updateData() override;
};

#endif // VECTORDATASETFORM_H
