#include "postgisconnectionform.h"
#include "ui_postgisconnectionform.h"
#include <QMessageBox>
#include "logger.h"
PostGISConnectionForm::PostGISConnectionForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PostGISConnectionForm)
{
    ui->setupUi(this);
    setWindowTitle("Подключение к PostGIS");
    //TESTING
    ui->lineEditDBName->setText("test");
    ui->lineEditDBHost->setText("127.0.0.1");
    ui->lineEditPassword->setText("1234");
    ui->lineEditDBPort->setText("5432");
    ui->lineEditUserName->setText("postgres");
}

PostGISConnectionForm::~PostGISConnectionForm()
{
    delete ui;
}

std::string PostGISConnectionForm::createConnectionStringFromUI() const
{
    // Получаем значения из QLineEdit
    QString host = ui->lineEditDBHost->text();
    QString port = ui->lineEditDBPort->text();
    QString dbName = ui->lineEditDBName->text();
    QString user = ui->lineEditUserName->text();
    QString password = ui->lineEditPassword->text();
    if (host.isEmpty() || port.isEmpty() || dbName.isEmpty() || user.isEmpty() || password.isEmpty())
        return "";
    return "PG:host=" + host.toStdString() +
            " port=" + port.toStdString() +
            " dbname='" + dbName.toStdString() + "'" +
            " user='" + user.toStdString() + "'" +
            " password='" + password.toStdString() + "'";
}

void PostGISConnectionForm::onConnectionStringValidated(bool isValid, const std::string &connectionString)
{
    mValid = isValid;
    mValid ? QMessageBox::information(this, "Информация", "Данные для подключения корректны") :
              QMessageBox::warning(this, "Ошибка", "Данные для подключения некорректны");

}

void PostGISConnectionForm::on_pushButtonConnect_clicked()
{
    auto str = createConnectionStringFromUI();
    if (str.empty())
    {
        QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
        return;
    }
    emit testConnectionRequested(str);
    if (mValid)
    {
        QString connectionName = ui->lineEditName->text();
        emit connectionStringReady(connectionName, str);
    }
}


void PostGISConnectionForm::on_pushButtonCancel_clicked()
{
    close();
}


void PostGISConnectionForm::on_pushButtonValidate_clicked()
{
    auto str = createConnectionStringFromUI();
    if (str.empty())
    {
        QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
        return;
    }
    emit testConnectionRequested(str);
}


void PostGISConnectionForm::on_pushButtonHidePas_clicked(bool checked)
{
    if (!checked)
    {
        ui->pushButtonHidePas->setIcon(QIcon(":/images/icons/dialogs/icon_show_black.png"));
        ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    }
    else
    {
        ui->pushButtonHidePas->setIcon(QIcon(":/images/icons/dialogs/icon_hide_black.png"));
        ui->lineEditPassword->setEchoMode(QLineEdit::Normal);
    }
}

