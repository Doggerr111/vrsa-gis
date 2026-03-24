#ifndef GEOSOPERATIONFORM_H
#define GEOSOPERATIONFORM_H

#include <QDialog>
#include "common/spatialoperation_dto.h"

namespace Ui {
class GeosOperationForm;
}

namespace vrsa {
namespace vector {
enum class SpatialOperationType: int;
}
}
class GeosOperationForm : public QDialog
{

    Q_OBJECT
public:
    using SpatialOperationType = vrsa::vector::SpatialOperationType;
    explicit GeosOperationForm(SpatialOperationType type, std::vector<std::string> layerNames,
                               QWidget *parent = nullptr);
    ~GeosOperationForm();
private:
    vrsa::common::SpatialOperationDTO createDTO();
private slots:
    void onFileButtonClicked(QString fileName);
    void on_pushButtonCancel_clicked();
    void on_pushButtonAccept_clicked();

signals:
    void operationAccepted(const vrsa::common::SpatialOperationDTO &dto);
private:
    Ui::GeosOperationForm *ui;
    SpatialOperationType mType;
};

#endif // GEOSOPERATIONFORM_H
