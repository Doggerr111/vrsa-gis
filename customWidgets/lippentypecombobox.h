#ifndef LIPPENTYPECOMBOBOX_H
#define LIPPENTYPECOMBOBOX_H

#include <QComboBox>

class LIPPenTypeComboBox : public QComboBox
{
public:
    LIPPenTypeComboBox(QWidget* parent=nullptr);
    Qt::PenStyle getStyle();
};

#endif // LIPPENTYPECOMBOBOX_H
