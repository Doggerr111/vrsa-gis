#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "services/giscontroller.h"
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionLoad_vector_layer_triggered();

    void on_crsComboBox_currentIndexChanged(int index);

    void on_pushButton_addFeature_clicked();

public slots:
    void updateScaleLineEdit(int mapScale, double widgetScale);
    void updateCoordinatesLineEdit(QPointF p);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<vrsa::services::GISController> mGisController;

};
#endif // MAINWINDOW_H
