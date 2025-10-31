#include "lipcombobox.h"

LIPComboBox::LIPComboBox(QWidget* parent)
    :   QComboBox(parent)
{
    int index=0;
//    foreach(LIPVectorLayer *layer, LIPProject::getInstance().getVectorLayers())
//    {
//        addItem(layer->returnGISName());
//        setItemData(index, layer->returnFileName());
//        index++;
//    }
}

//LIPVectorLayer *LIPComboBox::getVectorLayer()
//{
//    return LIPProject::getInstance().getVectorLayerByPath(itemData(currentIndex()).toString());
//}
