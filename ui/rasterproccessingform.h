#ifndef RASTERPROCCESSINGFORM_H
#define RASTERPROCCESSINGFORM_H

#include <QDialog>
#include "common/rasterprocessingparams.h"
namespace Ui {
class RasterProccessingForm;
}

class RasterProccessingForm : public QDialog
{
    Q_OBJECT

public:
    explicit RasterProccessingForm(vrsa::common::RasterOperationType type, const std::vector<std::string>& rasterNames,
                                   const std::vector<std::string>& vectorNames = {}, QWidget *parent = nullptr);
    ~RasterProccessingForm();

private:
    vrsa::common::RasterProcessingParams createParamsDTO();
    void setup();

private slots:
    void on_pushButtonAccept_clicked();
    void on_pushButtonCancel_clicked();
    void onFileButtonClicked(QString fileName);

signals:
    void operationAccepted(vrsa::common::RasterProcessingParams params);
private:
    Ui::RasterProccessingForm *ui;
    vrsa::common::RasterOperationType mOperationType;
};

#endif // RASTERPROCCESSINGFORM_H
