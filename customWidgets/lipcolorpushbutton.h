#ifndef LIPCOLORPUSHBUTTON_H
#define LIPCOLORPUSHBUTTON_H

#include <QPushButton>
#include <QColorDialog>
class LIPColorPushButton : public QPushButton
{
public:
    LIPColorPushButton(QWidget* parent = nullptr);
    QColor getColor();
private:
    QColor mColor;


    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // LIPCOLORPUSHBUTTON_H
