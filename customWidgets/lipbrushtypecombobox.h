#ifndef LIPBRUSHTYPECOMBOBOX_H
#define LIPBRUSHTYPECOMBOBOX_H

#include <QComboBox>

class LIPBrushTypeComboBox : public QComboBox
{
public:
    LIPBrushTypeComboBox(QWidget* parent=nullptr);
    Qt::BrushStyle getStyle();
};

#endif // LIPBRUSHTYPECOMBOBOX_H
