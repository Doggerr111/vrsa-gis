#include "crscombobox.h"
#include <QDebug>

CRSComboBox::CRSComboBox(QWidget* parent)
    : QComboBox(parent)
{

    for (const auto& crs: vrsa::georef::SpatialReferenceLibrary::getInstance().GetAvaliableSystems())
        addItem(QString::fromStdString(crs));

    //connect(&vrsa::georef::SpatialReferenceLibrary::getInstance(),SIGNAL(newCRS()),this,SLOT(updateCRS()));
    //QObject::connect(LIPProject::getInstance(), &LIPProject::newCRS, this, &LIPCRSComboBox::updateCRS);
    //connect(LIPProject::getInstance(), &LIPProject::newCRS, this, &LIPCRSComboBox::updateCRS);
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

void CRSComboBox::updateCRS()
{
//    clear();
//    foreach(LIPCoordinateSystem *crs, LIPProject::getInstance().getCoordinateSystems())
//    {
//        addItem(crs->getName());
//        //setItemData(index, layer->returnFileName());
//        //index++;
//    }
}

