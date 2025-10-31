#ifndef LIPATTRIBUTETABLEWIDGET_H
#define LIPATTRIBUTETABLEWIDGET_H

#include <QTableWidget>
//#include <vector/lipvectorlayer.h>
#include <QDebug>
//#include "lipattribute.h"
/** Виджет для отображения аттрибутивных таблиц векторных слоёв */
class LIPAttributeTableWidget : public QTableWidget
{
public:
    LIPAttributeTableWidget(QWidget* parent = nullptr);
    //void setLayer(LIPVectorLayer* layer);
    void setColumnNames();
    void setFields();
private:
    //LIPVectorLayer* mLayer;
};

#endif // LIPATTRIBUTETABLEWIDGET_H
