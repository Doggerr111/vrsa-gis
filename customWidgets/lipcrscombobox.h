#ifndef LIPCRSCOMBOBOX_H
#define LIPCRSCOMBOBOX_H

#include <QComboBox>
//#include "lipproject.h"
#include <QObject>
#include <geos/geom/LineSegment.h>
#include <geos/algorithm/Area.h>
class LIPCRSComboBox : public QComboBox
{
    Q_OBJECT
public:
    LIPCRSComboBox(QWidget* parent=nullptr);
    //LIPCoordinateSystem *getCurrentCRS();


public slots:
    void updateCRS();
};

#endif // LIPCRSCOMBOBOX_H
