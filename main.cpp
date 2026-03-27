#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <gdal_priv.h>
#include <iostream>
#include "common/logger.h"
#include <QDir>
#include <QRandomGenerator>
#include "common/applicationsettings.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file(":/app_styles/dark_2.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QString::fromUtf8(file.readAll());
        a.setStyleSheet(styleSheet);
        file.close();
        VRSA_INFO("UI", "Style sheet loaded successfully");
    }
    else
        VRSA_ERROR("UI", "Style sheet not loaded. Path:" + QFileInfo(file).absoluteFilePath().toStdString());
    vrsa::common::ApplicationSettings::instance();
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "VRSA_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            a.installTranslator(&translator);
            break;
        }
    }

    GDALAllRegister();
    vrsa::common::Logger::getInstance().setLogFile(QDir::currentPath().toStdString()+"/application.log");
    vrsa::common::Logger::getInstance().setLogLevel(vrsa::common::LogLevel::TRACE);

    qRegisterMetaTypeStreamOperators<vrsa::common::TreeItemType>("vrsa::common::CustomRoles");
    qRegisterMetaTypeStreamOperators<vrsa::common::TreeItemType>("vrsa::common::GeometryType");
    qRegisterMetaTypeStreamOperators<vrsa::common::TreeItemType>("vrsa::common::DatasetType");
    qRegisterMetaTypeStreamOperators<vrsa::common::TreeItemType>("vrsa::common::TreeItemType");
    qRegisterMetaTypeStreamOperators<vrsa::common::TreeItemType>("vrsa::common::StyleParametr");
    MainWindow w;
    w.show();
    return a.exec();

}
