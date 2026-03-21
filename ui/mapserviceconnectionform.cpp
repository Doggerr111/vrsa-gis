#include "mapserviceconnectionform.h"
#include "ui_mapserviceconnectionform.h"
#include "common/GisDefines.h"
#include <QMessageBox>
MapServiceConnectionForm::MapServiceConnectionForm(WebServiceType type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapServiceConnectionForm),
    mServiceType{type},
    mValid{false}
{
    ui->setupUi(this);
    ui->lineEditURL->setText("https://tile.openstreetmap.org/${z}/${x}/${y}.png");
    setWindowTitle("Подключение к картографическому сервису:" + webServiceTypeToString(type));
}

MapServiceConnectionForm::~MapServiceConnectionForm()
{
    delete ui;
}

std::string MapServiceConnectionForm::createXMLFromUI() const
{
    QString tileUrl = ui->lineEditURL->text();


    bool yOriginTop = false; //xyz = top, tms = bottom
    switch (mServiceType)
    {
    case WebServiceType::XYZ:
    {
        if (tileUrl.isEmpty()) return "";
        yOriginTop = true;
        // Собираем XML из параметров
        std::string xml = "<GDAL_WMS>\n";
        //URL из UI
        xml += "    <Service name=\"TMS\">\n";
        xml += "        <ServerUrl>" + tileUrl.toStdString() + "</ServerUrl>\n";
        xml += "    </Service>\n";
        //пока константа
        xml += "    <DataWindow>\n";
        xml += "        <UpperLeftX>-20037508.34</UpperLeftX>\n";
        xml += "        <UpperLeftY>20037508.34</UpperLeftY>\n";
        xml += "        <LowerRightX>20037508.34</LowerRightX>\n";
        xml += "        <LowerRightY>-20037508.34</LowerRightY>\n";
        xml += "        <TileLevel>18</TileLevel>\n";
        xml += "        <TileCountX>1</TileCountX>\n";
        xml += "        <TileCountY>1</TileCountY>\n";
        //зависит от типа (XYZ = top, TMS = bottom)
        if (yOriginTop)
            xml += "        <YOrigin>top</YOrigin>\n";
        else
            xml += "        <YOrigin>bottom</YOrigin>\n";

        xml += "    </DataWindow>\n";

        //пока фикс
        xml += "    <Projection>EPSG:3857</Projection>\n";
        xml += "    <BlockSizeX>256</BlockSizeX>\n";
        xml += "    <BlockSizeY>256</BlockSizeY>\n";
        xml += "    <BandsCount>3</BandsCount>\n";

        //кэш
        xml += "    <Cache/>\n";

        xml += "</GDAL_WMS>";

        return xml;
    }
    default:
        return "";
    }
}

void MapServiceConnectionForm::on_pushButtonConnect_clicked()
{
    auto str = createXMLFromUI();
    if (str.empty())
    {
        QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
        return;
    }
    emit testConnectionRequested(str);
    if (mValid)
    {
        QString connectionName = ui->lineEditConnectionName->text();
        emit connectionXMLReady(connectionName, str);
    }
}


void MapServiceConnectionForm::on_pushButtonCancel_clicked()
{
    close();
}


void MapServiceConnectionForm::on_pushButtonValidate_clicked()
{
    qDebug()<< QString::fromStdString(createXMLFromUI());
    emit testConnectionRequested(createXMLFromUI());
}

void MapServiceConnectionForm::onConnectionValidated(bool valid, const std::string &str)
{
    mValid = valid;
    mValid ? QMessageBox::information(this, "Информация", "Данные для подключения к " +
                                      vrsa::common::webServiceTypeToString(mServiceType) + " корректны") :
              QMessageBox::warning(this, "Ошибка", "Данные для подключения к "+
                                   vrsa::common::webServiceTypeToString(mServiceType) + " некорректны");
}

