#ifndef MAPSERVICECONNECTIONFORM_H
#define MAPSERVICECONNECTIONFORM_H

#include <QDialog>

namespace Ui {
class MapServiceConnectionForm;
}

namespace vrsa{
namespace common{
enum class WebServiceType: int;
}
}

class MapServiceConnectionForm : public QDialog
{
    Q_OBJECT
using WebServiceType = vrsa::common::WebServiceType;
public:
    explicit MapServiceConnectionForm(WebServiceType type, QWidget *parent = nullptr);
    ~MapServiceConnectionForm();

private:
    std::string createXMLFromUI() const;
signals:
    void testConnectionRequested(const std::string& connectionString);
    void connectionXMLReady(const QString& connectionName, const std::string& connectionString);
public slots:
    void onConnectionValidated(bool valid, const std::string& str);
private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonCancel_clicked();
    void on_pushButtonValidate_clicked();

private:
    Ui::MapServiceConnectionForm *ui;
    WebServiceType mServiceType;
    bool mValid;
};

#endif // MAPSERVICECONNECTIONFORM_H
