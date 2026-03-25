#ifndef ATTRIBUTETABLEFORM_H
#define ATTRIBUTETABLEFORM_H
#include <QDialog>

namespace vrsa{
namespace vector{
class VectorLayer;
class VectorFeature;
}
}
namespace Ui {
class AttributeTableForm;
}

class AttributeTableForm : public QDialog
{
    Q_OBJECT
using VectorLayer = vrsa::vector::VectorLayer;
public:
    explicit AttributeTableForm(VectorLayer* = nullptr, QWidget *parent = nullptr);
    ~AttributeTableForm();
    inline void setLayer(VectorLayer* vL) noexcept { mLayer = vL; }
private:
    void setAttributeTable() const;
private slots:
    void on_attributeTable_cellChanged(int row, int column);
    void on_attributeTable_itemSelectionChanged();
    void on_toolButtonSelectAll_clicked();
    void on_toolButtonDeselectAll_clicked();
    void on_toolButtonInvertSelection_clicked();

    void on_toolButtonDeleteSelectedFeatures_clicked();

private:
    Ui::AttributeTableForm *ui;
    VectorLayer* mLayer;
};

#endif // ATTRIBUTETABLEFORM_H
