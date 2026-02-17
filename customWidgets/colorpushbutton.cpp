#include "colorpushbutton.h"

ColorPushButton::ColorPushButton(QWidget* parent)
    :QPushButton(parent)
{
    mColor = Qt::black;
    setText("Выбрать цвет");
}

QColor ColorPushButton::getColor()
{
    return mColor;
}


void ColorPushButton::mousePressEvent(QMouseEvent *event)
{
    mColor = QColorDialog::getColor();
    if (!mColor.isValid())
        mColor = Qt::black;
    QPushButton::mousePressEvent(event);
    emit clicked();
}
