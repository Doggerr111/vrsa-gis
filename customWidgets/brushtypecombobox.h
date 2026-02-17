#ifndef LIPBRUSHTYPECOMBOBOX_H
#define LIPBRUSHTYPECOMBOBOX_H

#include <QComboBox>

class BrushTypeComboBox : public QComboBox
{
public:
    BrushTypeComboBox(QWidget* parent=nullptr);
    Qt::BrushStyle getStyle();
};

#endif // LIPBRUSHTYPECOMBOBOX_H
