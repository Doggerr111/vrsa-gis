#ifndef ATTRIBUTETABLEFORM_H
#define ATTRIBUTETABLEFORM_H

#include <QDialog>
#include "vector/vectorlayer.h"
#include "common/GisDefines.h"
namespace Ui {
class AttributeTableForm;
}

class AttributeTableForm : public QDialog
{
    Q_OBJECT
using vectorLayer = vrsa::vector::VectorLayer;
public:
    explicit AttributeTableForm(QWidget *parent = nullptr, vectorLayer* = nullptr);
    ~AttributeTableForm();

    inline void setLayer(vectorLayer* vL) noexcept
    {
        mLayer = vL;
    }

private slots:
    void on_attributeTable_cellChanged(int row, int column);

private:
    void setAttributeTable() const;

    QVariant attributeValueToQVariant(const vrsa::vector::VectorFeature::AttributeValue& value) const;
private:
    Ui::AttributeTableForm *ui;
    vectorLayer* mLayer;
};

#endif // ATTRIBUTETABLEFORM_H
