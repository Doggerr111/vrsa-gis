#ifndef LIPTREEWIDGETITEM_H
#define LIPTREEWIDGETITEM_H

#include <QTreeWidgetItem>

class TreeWidgetItem : public QTreeWidgetItem
{
public:
    TreeWidgetItem();
    void setFileName(QString path);

private:
    QString fileName;
};





#endif // LIPTREEWIDGETITEM_H
