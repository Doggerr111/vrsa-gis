#include "lipcolorpushbutton.h"

LIPColorPushButton::LIPColorPushButton(QWidget* parent)
    :QPushButton(parent)
{
    mColor = Qt::white;
    setText("Выбрать цвет");
}

QColor LIPColorPushButton::getColor()
{
    return mColor;
}


void LIPColorPushButton::mousePressEvent(QMouseEvent *event)
{
    mColor = QColorDialog::getColor();
    if (!mColor.isValid())
        mColor = Qt::white;
    QPushButton::mousePressEvent(event);
    emit clicked();
}
