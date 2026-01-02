#ifndef BASEDATASETFORM_H
#define BASEDATASETFORM_H
#include <QDialog>
#include <QTreeWidgetItem>
#include "GisDefines.h"
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QMouseEvent>
#include <QMessageBox>
class BaseDatasetForm: public QDialog
{
    Q_OBJECT
public:
    BaseDatasetForm(QWidget *parent = nullptr);

    // Реализация чисто виртуальных методов
    virtual void loadFromItem(QTreeWidgetItem* item);
    //virtual void updateData();
    //virtual void saveToItem(QTreeWidgetItem* item) = 0;

protected:
    QString mFilePath;
};

#endif // BASEDATASETFORM_H
