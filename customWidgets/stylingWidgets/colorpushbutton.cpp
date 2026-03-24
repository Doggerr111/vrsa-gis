#include "colorpushbutton.h"
#include <QPainter>
#include <simplepointsymbol.h>
#include <simplelinesymbol.h>
#include <simplepolygonsymbol.h>
#include <QColorDialog>

ColorPushButton::ColorPushButton(const Symbol *symbol, StyleParam param, QWidget *parent)
    : QPushButton(parent),
      mColor{Qt::black}
{
    if (symbol)
    {
        switch (symbol->type())
        {
        case vrsa::common::SymbolType::SimplePointSymbol:
        {
            auto pointS = static_cast<const vrsa::graphics::SimplePointSymbol*>(symbol);
            if (param == StyleParam::PenColor)
                mColor = pointS->borderColor;
            else if (param == StyleParam::BrushColor)
                mColor = pointS->fillColor;
            break;
        }
        case vrsa::common::SymbolType::SimpleLineSymbol:
        {
            auto pointS = static_cast<const vrsa::graphics::SimpleLineSymbol*>(symbol);
            if (param == StyleParam::PenColor)
                mColor = pointS->borderColor;
            break;
        }
        case vrsa::common::SymbolType::SimplePolygonSymbol:
        {
            auto pointS = static_cast<const vrsa::graphics::SimplePolygonSymbol*>(symbol);
            if (param == StyleParam::PenColor)
                mColor = pointS->borderColor;
            else if (param == StyleParam::BrushColor)
                mColor = pointS->fillColor;
            break;
        }
        default:
            mColor = Qt::black;
        }
    }
}


void ColorPushButton::mousePressEvent(QMouseEvent *event)
{
    QColor oldColor = mColor;
    QColor newColor = QColorDialog::getColor(mColor, this, tr("Выберите цвет"));
    if (newColor.isValid())
    {
        mColor = newColor;
        update();
        emit clicked();
    }
    QPushButton::mousePressEvent(event);
}

void ColorPushButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //тень
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 60));
    painter.drawRoundedRect(5, 6, 20, height() - 10, 3, 3);

    QLinearGradient gradient(5, 5, 5, height() - 5);
    gradient.setColorAt(0, mColor.lighter(120));
    gradient.setColorAt(1, mColor.darker(110));
    painter.setBrush(gradient);
    painter.setPen(QPen(QColor(80, 80, 80), 1));
    painter.drawRoundedRect(5, 5, 40, height() - 10, 4, 4);

    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);
    //цвет текста в зависимости от состояния кнопки
    if (isDown() || isChecked())
        painter.setPen(QColor(255, 255, 255));
    else if (underMouse())
        painter.setPen(QColor(184, 92, 26));
    else
        painter.setPen(QColor(220, 220, 220));

    painter.drawText(52, 0, width() - 40, height(),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     mColor.name().toUpper());
}
