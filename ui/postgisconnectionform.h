#ifndef POSTGISCONNECTIONFORM_H
#define POSTGISCONNECTIONFORM_H

#include <QDialog>

namespace Ui {
class PostGISConnectionForm;
}

class PostGISConnectionForm : public QDialog
{
    Q_OBJECT
public:
    explicit PostGISConnectionForm(QWidget *parent = nullptr);
    ~PostGISConnectionForm();
signals:
    void testConnectionRequested(const std::string& connectionString);
    void connectionStringReady(const QString& connectionName, const std::string& connectionString);
private:
    std::string createConnectionStringFromUI() const;
public slots:
    void onConnectionStringValidated(bool isValid, const std::string& connectionString);
private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonCancel_clicked();
    void on_pushButtonValidate_clicked();


    void on_pushButtonHidePas_clicked(bool checked);

private:
    Ui::PostGISConnectionForm *ui;
    bool mValid;
};

#endif // POSTGISCONNECTIONFORM_H
