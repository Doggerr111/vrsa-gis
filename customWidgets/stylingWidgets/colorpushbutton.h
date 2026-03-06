#ifndef LIPCOLORPUSHBUTTON_H
#define LIPCOLORPUSHBUTTON_H

#include <QPushButton>
#include <QColorDialog>
#include <symbol.h>
class ColorPushButton : public QPushButton
{
    using Symbol = vrsa::graphics::Symbol;
    using StyleParam = vrsa::common::StyleParametr;
    Q_OBJECT
public:
    ColorPushButton(const Symbol *symbol, StyleParam param, QWidget* parent = nullptr);
    QColor getColor();
private:
    QColor mColor;


    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent*) override;

private slots:
};

#endif // LIPCOLORPUSHBUTTON_H
