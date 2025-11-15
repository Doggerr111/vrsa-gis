#include "treewidgetitem.h"

TreeWidgetItem::TreeWidgetItem()
{

}

void TreeWidgetItem::setFileName(QString path)
{
    if (!path.isEmpty())
        fileName=path;
}
