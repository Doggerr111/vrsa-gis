#include "colorpushbutton.h"
#include <QPainter>
#include <simplepointsymbol.h>
#include <simplelinesymbol.h>
#include <simplepolygonsymbol.h>

ColorPushButton::ColorPushButton(const Symbol *symbol, StyleParam param, QWidget *parent)
{
    mColor = Qt::black;
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

void ColorPushButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    // Рисуем цветной квадрат
    painter.setPen(Qt::black);
    painter.setBrush(mColor);
    painter.drawRect(5, 5, 20, height() - 10);
    // Рисуем текст (название цвета или HEX)
    painter.setPen(Qt::black);
    painter.drawText(30, 0, width() - 30, height(),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     mColor.name());
}
