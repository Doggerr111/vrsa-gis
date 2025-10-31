#include "lippentypecombobox.h"

LIPPenTypeComboBox::LIPPenTypeComboBox(QWidget *parent)
    : QComboBox(parent)
{
    addItem("Сплошная линия");
    addItem("Пунктирная линия");
    addItem("Точечная линия");
    addItem("Штрихпунктирная линия");
    addItem("ШТрихпунктирная линия с 2 точками");
}

Qt::PenStyle LIPPenTypeComboBox::getStyle()
{
    switch (currentIndex())
    {
    case 0:
        return Qt::SolidLine;
    case 1:
        return Qt::DashLine;
    case 2:
        return Qt::DotLine;
    case 3:
        return Qt::DashDotLine;
    case 4:
        return Qt::DashDotDotLine;
    }
}
