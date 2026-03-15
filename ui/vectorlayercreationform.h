#ifndef VECTORLAYERCREATIONFORM_H
#define VECTORLAYERCREATIONFORM_H

#include <QDialog>
#include <QMap>
#include "common/layercreationparams.h"
namespace Ui {
class VectorLayerCreationForm;
}
namespace vrsa
{
namespace common
{
enum class GeometryType: int;
}
}

class VectorLayerCreationForm : public QDialog
{
    Q_OBJECT
    using GeometryType = vrsa::common::GeometryType;
public:
    explicit VectorLayerCreationForm(QWidget *parent = nullptr);
    explicit VectorLayerCreationForm(vrsa::common::GeometryType geomType, QWidget *parent = nullptr);
    ~VectorLayerCreationForm();
private:
    void setupTableWidget();
    void updateRowNumbers();
    vrsa::common::LayerDefinition createLayerDefinition();
    std::vector<vrsa::common::FieldDefinition> createFieldDefenition();
    std::unordered_map<std::string, std::string> parseGdalOptions();
private:
    Ui::VectorLayerCreationForm *ui;
    int mGeomType;


signals:
    void layerCreationAccepted(const vrsa::common::LayerDefinition &layerDef);
private slots:
    void onFileButtonClicked(QString fileName);
    void on_toolButtonAddAttribute_clicked();
    void on_toolButtonRemoveAttribute_clicked();
    void on_pushButtonCancel_clicked();
    void on_pushButtonCreateLayer_clicked();

    // QWidget interface
//protected:
//    void resizeEvent(QResizeEvent *event) override;
    void on_comboBoxGeometry_currentIndexChanged(int index);
};

#endif // VECTORLAYERCREATIONFORM_H
