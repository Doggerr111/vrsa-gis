#ifndef VECTORREPROJECTIONFORM_H
#define VECTORREPROJECTIONFORM_H

#include <QDialog>

namespace Ui {
class VectorReprojectionForm;
}

class VectorReprojectionForm : public QDialog
{
    Q_OBJECT
public:
    explicit VectorReprojectionForm(std::vector<std::string> layerNames, QWidget *parent = nullptr);
    ~VectorReprojectionForm();


signals:
    void reprojectingAccepted(const std::string& layer, const std::string& crs,
                              const std::string& destinationPath);
private slots:
    void on_pushButtonAccept_clicked();
    void on_pushButtonCancel_clicked();
    void onFileButtonClicked(QString fileName);



private:
    Ui::VectorReprojectionForm *ui;
};

#endif // VECTORREPROJECTIONFORM_H
