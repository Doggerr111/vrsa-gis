#include "filedialogbutton.h"
#include "QFileDialog"

FileDialogButton::FileDialogButton(QWidget *parent)
    : QPushButton(parent),
      mRole{}
{
    setIcon(QIcon(":/images/icons/diskette.png"));
}

void FileDialogButton::mousePressEvent(QMouseEvent *event)
{

    QFileDialog dialog(this);
    QString fileName;
    switch (mRole)
    {
    case ButtonRole::NewLayer:
    {
        dialog.setNameFilter(tr("Vector formats (*.shp *.geojson *.gpkg)"));
        fileName = dialog.getSaveFileName(
                    this,
                    "Save Vector Layer",
                    QDir::homePath() + "/untitled",
                    "Vector formats (*.shp *.geojson *.gpkg)"
                );

        break;
    }
    case ButtonRole::OpenDataset:
    case ButtonRole::ImportLayer:
    {
        fileName = QFileDialog::getOpenFileName(
            this,
            tr("Open File"),
            QDir::homePath(),
            tr("Shapefiles (*.shp);;All Files (*)")
        );


        break;
    }
    case ButtonRole::SaveDataset:
    case ButtonRole::ExportLayer:
        fileName = dialog.getSaveFileName(
                    this,
                    "Save Vector Layer",
                    QDir::homePath() + "/untitled.shp",
                    tr("Vector formats (*.shp *.geojson *.gpkg)")
                );
        break;

    }
    emit clicked(fileName);

}
