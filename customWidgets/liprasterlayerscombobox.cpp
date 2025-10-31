#include "liprasterlayerscombobox.h"

LIPRasterLayersComboBox::LIPRasterLayersComboBox(QWidget* parent)
    : QComboBox(parent)
{
    int index=0;
//    foreach(LIPRasterLayer *layer, LIPProject::getInstance().getRasterLayers())
//    {
//        addItem(layer->getGISName());
//        setItemData(index, layer->getFileName());
//        index++;
//    }
}

//LIPRasterLayer *LIPRasterLayersComboBox::getRasterLayer()
//{
//    return LIPProject::getInstance().getRasterLayerByPath(itemData(currentIndex()).toString());
//}
