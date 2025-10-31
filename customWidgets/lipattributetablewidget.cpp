#include "lipattributetablewidget.h"

LIPAttributeTableWidget::LIPAttributeTableWidget(QWidget* parent)
    : QTableWidget(parent)//,
      //mLayer{nullptr}
{
//    if (mLayer!=nullptr)
//    {
//        setColumnNames();
//    }
}

//void LIPAttributeTableWidget::setLayer(LIPVectorLayer *layer)
//{
//    mLayer=layer;
//    if (mLayer!=nullptr)
//    {
//        setColumnNames();
//        setFields();
//    }
//}

//void LIPAttributeTableWidget::setColumnNames()
//{
//    if (mLayer!=nullptr)
//    {
//        //QStringList mList = mLayer->getAttributesNamesAsList();
//        //qDebug()<< mList;
//        setColumnCount(mLayer->getAttributesNamesAsList().count());
//        setHorizontalHeaderLabels(mLayer->getAttributesNamesAsList());
//    }
//}

//void LIPAttributeTableWidget::setFields()
//{
//    if (mLayer!=nullptr)
//    {
//        std::map<int, QVector<LIPAttribute>> fields = mLayer->getAttributes();
//        for (const auto& item : fields)
//            {
//                int currentRow = item.first;                            // Получаем значение типа int
//                insertRow(currentRow);
//                const QVector<LIPAttribute>& myVector = item.second;  // Получаем значение типа QVector
//                int i=0;
//                for (auto& attribute : myVector)
//                {
//                    QTableWidgetItem *item = new QTableWidgetItem(attribute.getValue().toString());
//                    setItem(currentRow, i, item);
//                    i++;

//                }
//            }
//    }
//}
