#include "lipcrscombobox.h"
#include <QDebug>
#include <geos/triangulate/DelaunayTriangulationBuilder.h>

LIPCRSComboBox::LIPCRSComboBox(QWidget* parent)
    : QComboBox(parent)
{

//    connect(&LIPProject::getInstance(),SIGNAL(newCRS()),this,SLOT(updateCRS()));
//    //QObject::connect(LIPProject::getInstance(), &LIPProject::newCRS, this, &LIPCRSComboBox::updateCRS);

//    //connect(LIPProject::getInstance(), &LIPProject::newCRS, this, &LIPCRSComboBox::updateCRS);
//    int index=0;
//    foreach(LIPCoordinateSystem *crs, LIPProject::getInstance().getCoordinateSystems())
//    {
//        addItem(crs->getName());
//        //setItemData(index, layer->returnFileName());
//        index++;
//    }
        //using namespace geos::geom;
//    geos::algorithm::Area ar;

    //    cr.setNull();
}

/*LIPCoordinateSystem *LIPCRSComboBox::getCurrentCRS()
{
    for (LIPCoordinateSystem *crs: LIPProject::getInstance().getCoordinateSystems())
    {
        if (crs->getName()==currentText())
        {
            return crs;
        }
    }
    return nullptr;
}*/

void LIPCRSComboBox::updateCRS()
{
//    clear();
//    foreach(LIPCoordinateSystem *crs, LIPProject::getInstance().getCoordinateSystems())
//    {
//        addItem(crs->getName());
//        //setItemData(index, layer->returnFileName());
//        //index++;
//    }
}

