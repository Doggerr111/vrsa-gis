#ifndef LIPPENTYPECOMBOBOX_H
#define LIPPENTYPECOMBOBOX_H

#include <QComboBox>

class PenTypeComboBox : public QComboBox
{
public:
    PenTypeComboBox(QWidget* parent=nullptr);
    Qt::PenStyle getStyle();
};

#endif // LIPPENTYPECOMBOBOX_H
