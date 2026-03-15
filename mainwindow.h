#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "services/giscontroller.h"
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class QButtonGroup;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void initialize();
    void setupLogger();
    vrsa::services::ViewComponents createViewComponents();

private slots:
    void on_actionLoad_vector_layer_triggered();
    void on_crsComboBox_currentIndexChanged(int index);
    void on_pushButton_addFeature_clicked();
    void on_pushButtonSingleSelection_clicked(bool checked);
    void on_pushButton_testVectorLayerUtils_clicked();


private:
    Ui::MainWindow *ui;
    std::unique_ptr<vrsa::services::GISController> mGisController;
    QButtonGroup* mMapToolExclusiveGroup;
    //QSplitter *spliter;
};
#endif // MAINWINDOW_H
