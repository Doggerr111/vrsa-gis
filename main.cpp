#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <gdal_priv.h>
#include <iostream>
#include "common/logger.h"
#include <QDir>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "VRSA_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    GDALAllRegister();
    std::cout << "GDAL works!" << std::endl;
    vrsa::common::Logger::getInstance().setLogFile(QDir::currentPath().toStdString()+"/application.log");
    vrsa::common::Logger::getInstance().setLogLevel(vrsa::common::LogLevel::TRACE);
    VRSA_DEBUG("GDAL", "Opening file");

    return 0;

    MainWindow w;
    w.show();
    return a.exec();
}
