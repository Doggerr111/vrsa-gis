#include "lipbrushtypecombobox.h"

LIPBrushTypeComboBox::LIPBrushTypeComboBox(QWidget *parent)
    : QComboBox(parent)
{
    addItem("SolidPattern");
    addItem("Dense1Pattern");
    addItem("Dense2Pattern");
    addItem("Dense3Pattern");
    addItem("Dense4Pattern");
    addItem("Dense5Pattern");
    addItem("Dense6Pattern");
    addItem("Dense7Pattern");
    addItem("Без заливки");
    addItem("HorPattern");
    addItem("VerPattern");
    addItem("CrossPattern");
    addItem("BDiagPattern");
    addItem("FDiagPattern");
    addItem("DiagCrossPattern");
}

Qt::BrushStyle LIPBrushTypeComboBox::getStyle()
{
    switch (currentIndex())
    {
    case 0:
        return Qt::SolidPattern;
    case 1:
        return Qt::Dense1Pattern;
    case 2:
        return Qt::Dense2Pattern;
    case 3:
        return Qt::Dense3Pattern;
    case 4:
        return Qt::Dense4Pattern;
    case 5:
        return Qt::Dense5Pattern;
    case 6:
        return Qt::Dense6Pattern;
    case 7:
        return Qt::Dense7Pattern;
    case 8:
        return Qt::NoBrush;
    case 9:
        return Qt::HorPattern;
    case 10:
        return Qt::VerPattern;
    case 11:
        return Qt::CrossPattern;
    case 12:
        return Qt::BDiagPattern;
    case 13:
        return Qt::FDiagPattern;
    case 14:
        return Qt::DiagCrossPattern;
    }
    return Qt::SolidPattern;
}
