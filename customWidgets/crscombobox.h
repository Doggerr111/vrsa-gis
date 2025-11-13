#ifndef LIPCRSCOMBOBOX_H
#define LIPCRSCOMBOBOX_H

#include <QComboBox>
#include <QObject>
#include "georef/spatialreferencelibrary.h"
class CRSComboBox : public QComboBox
{
    Q_OBJECT
public:
    CRSComboBox(QWidget* parent=nullptr);
    //LIPCoordinateSystem *getCurrentCRS();



public slots:
    void updateCRS();
};

#endif // LIPCRSCOMBOBOX_H
