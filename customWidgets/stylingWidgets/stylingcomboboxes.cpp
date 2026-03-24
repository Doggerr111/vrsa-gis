#include "stylingcomboboxes.h"
#include "simplepointsymbol.h"
#include "simplelinesymbol.h"
#include "simplepolygonsymbol.h"
#include <QPainter>

PenTypeComboBox::PenTypeComboBox(const Symbol* symbol, QWidget *parent)
    : QComboBox(parent)
{
    addItem(tr("Сплошная линия"));
    addItem(tr("Пунктирная линия"));
    addItem(tr("Точечная линия"));
    addItem(tr("Штрихпунктирная линия"));
    addItem(tr("Штрихпунктирная линия с 2 точками"));
    addItem(tr("Не использовать"));
    if (symbol)
    {
        switch (symbol->type())
        {
        case vrsa::common::SymbolType::SimplePointSymbol:
            setStyle(static_cast<const vrsa::graphics::SimplePointSymbol*>(symbol)->borderStyle);
            break;
        case vrsa::common::SymbolType::SimpleLineSymbol:
            setStyle(static_cast<const vrsa::graphics::SimpleLineSymbol*>(symbol)->borderStyle);
            break;
        case vrsa::common::SymbolType::SimplePolygonSymbol:
            setStyle(static_cast<const vrsa::graphics::SimplePolygonSymbol*>(symbol)->borderStyle);
            break;
        default:
            break;
        }
    }
}

Qt::PenStyle PenTypeComboBox::getStyle() const
{
    switch (currentIndex())
    {
    case 0:
        return Qt::SolidLine;
    case 1:
        return Qt::DashLine;
    case 2:
        return Qt::DotLine;
    case 3:
        return Qt::DashDotLine;
    case 4:
        return Qt::DashDotDotLine;
    case 5:
        return Qt::NoPen;
    }
    return Qt::SolidLine;
}

void PenTypeComboBox::setStyle(Qt::PenStyle style)
{
    switch (style)
    {
    case Qt::SolidLine:
        setCurrentIndex(0);
        break;
    case Qt::DashLine:
        setCurrentIndex(1);
        break;
    case Qt::DotLine:
        setCurrentIndex(2);
        break;
    case Qt::DashDotLine:
        setCurrentIndex(3);
        break;
    case Qt::DashDotDotLine:
        setCurrentIndex(4);
        break;
    case Qt::NoPen:
        setCurrentIndex(5);
        break;
    default:
        break;
    }
}


PenJoinComboBox::PenJoinComboBox(const Symbol *symbol, QWidget *parent)
    :QComboBox(parent)
{
    addItem(tr("Острый угол"));
    addItem(tr("Скошенный угол"));
    addItem(tr("Скругленный угол"));
    if (symbol)
    {
        switch (symbol->type())
        {
        case vrsa::common::SymbolType::SimplePointSymbol:
            setStyle(static_cast<const vrsa::graphics::SimplePointSymbol*>(symbol)->joinStyle);
            break;
        case vrsa::common::SymbolType::SimpleLineSymbol:
            setStyle(static_cast<const vrsa::graphics::SimpleLineSymbol*>(symbol)->joinStyle);
            break;
        case vrsa::common::SymbolType::SimplePolygonSymbol:
            setStyle(static_cast<const vrsa::graphics::SimplePolygonSymbol*>(symbol)->joinStyle);
            break;
        default:
            break;
        }
    }
}

Qt::PenJoinStyle PenJoinComboBox::getStyle() const
{
    switch (currentIndex())
    {
    case 0:
        return Qt::PenJoinStyle::MiterJoin;
    case 1:
        return Qt::PenJoinStyle::BevelJoin;
    case 2:
        return Qt::PenJoinStyle::RoundJoin;
    }
    return Qt::PenJoinStyle::RoundJoin;
}

void PenJoinComboBox::setStyle(Qt::PenJoinStyle style)
{
    switch (style)
    {
    case Qt::PenJoinStyle::MiterJoin:
        setCurrentIndex(0);
        break;
    case Qt::PenJoinStyle::BevelJoin:
        setCurrentIndex(1);
        break;
    case Qt::PenJoinStyle::RoundJoin:
        setCurrentIndex(2);
        break;
    default:
        break;
    }
}


BrushTypeComboBox::BrushTypeComboBox(const Symbol* symbol, QWidget *parent)
    : QComboBox(parent)
{
    addItem("SolidPattern");
    addItem("Dense1Pattern");
    addItem("Dense2Pattern");
    addItem("Dense3Pattern");
    addItem("Dense4Pattern");
    addItem("Dense5Pattern");
    addItem("Dense6Pattern");
    addItem("Dense7Pattern");
    addItem(tr("Без заливки"));
    addItem("HorPattern");
    addItem("VerPattern");
    addItem("CrossPattern");
    addItem("BDiagPattern");
    addItem("FDiagPattern");
    addItem("DiagCrossPattern");
    if (symbol)
    {
        if (symbol->type() == vrsa::common::SymbolType::SimplePointSymbol)
            setStyle(static_cast<const vrsa::graphics::SimplePointSymbol*>(symbol)->fillStyle);
        else if (symbol->type() == vrsa::common::SymbolType::SimplePolygonSymbol)
            setStyle(static_cast<const vrsa::graphics::SimplePolygonSymbol*>(symbol)->fillStyle);
    }
}

Qt::BrushStyle BrushTypeComboBox::getStyle() const
{
    switch (currentIndex())
    {
    case 0:
        return Qt::BrushStyle::SolidPattern;
    case 1:
        return Qt::BrushStyle::Dense1Pattern;
    case 2:
        return Qt::BrushStyle::Dense2Pattern;
    case 3:
        return Qt::BrushStyle::Dense3Pattern;
    case 4:
        return Qt::BrushStyle::Dense4Pattern;
    case 5:
        return Qt::BrushStyle::Dense5Pattern;
    case 6:
        return Qt::BrushStyle::Dense6Pattern;
    case 7:
        return Qt::BrushStyle::Dense7Pattern;
    case 8:
        return Qt::BrushStyle::NoBrush;
    case 9:
        return Qt::BrushStyle::HorPattern;
    case 10:
        return Qt::BrushStyle::VerPattern;
    case 11:
        return Qt::BrushStyle::CrossPattern;
    case 12:
        return Qt::BrushStyle::BDiagPattern;
    case 13:
        return Qt::BrushStyle::FDiagPattern;
    case 14:
        return Qt::BrushStyle::DiagCrossPattern;

    }
    return Qt::BrushStyle::SolidPattern;
}

void BrushTypeComboBox::setStyle(Qt::BrushStyle style)
{
    switch (style)
    {
    case Qt::BrushStyle::SolidPattern:
        setCurrentIndex(0);
        break;
    case Qt::BrushStyle::Dense1Pattern:
        setCurrentIndex(1);
        break;
    case Qt::BrushStyle::Dense2Pattern:
        setCurrentIndex(2);
        break;
    case Qt::BrushStyle::Dense3Pattern:
        setCurrentIndex(3);
        break;
    case Qt::BrushStyle::Dense4Pattern:
        setCurrentIndex(4);
        break;
    case Qt::BrushStyle::Dense5Pattern:
        setCurrentIndex(5);
        break;
    case Qt::BrushStyle::Dense6Pattern:
        setCurrentIndex(6);
        break;
    case Qt::BrushStyle::Dense7Pattern:
        setCurrentIndex(7);
        break;
    case Qt::BrushStyle::NoBrush:
        setCurrentIndex(8);
        break;
    case Qt::BrushStyle::HorPattern:
        setCurrentIndex(9);
        break;
    case Qt::BrushStyle::VerPattern:
        setCurrentIndex(10);
        break;
    case Qt::BrushStyle::CrossPattern:
        setCurrentIndex(11);
        break;
    case Qt::BrushStyle::BDiagPattern:
        setCurrentIndex(12);
        break;
    case Qt::BrushStyle::FDiagPattern:
        setCurrentIndex(13);
        break;
    case Qt::BrushStyle::DiagCrossPattern:
        setCurrentIndex(14);
        break;
    default:
        break;
    }
}

PenCapComboBox::PenCapComboBox(const Symbol *symbol, QWidget *parent)
    : QComboBox(parent)
{
    addItem(tr("Плоское окончание"));
    addItem(tr("Квадратное окончание"));
    addItem(tr("Круглое окончание"));
    if (symbol)
    {
        if (symbol->type() == vrsa::common::SymbolType::SimpleLineSymbol)
            setStyle(static_cast<const vrsa::graphics::SimpleLineSymbol*>(symbol)->capStyle);
    }


}

Qt::PenCapStyle PenCapComboBox::getStyle() const
{
    switch (currentIndex())
    {
    case 0:
        return Qt::PenCapStyle::FlatCap;
    case 1:
        return Qt::PenCapStyle::SquareCap;
    case 2:
        return Qt::PenCapStyle::RoundCap;
    }
    return Qt::PenCapStyle::RoundCap;
}

void PenCapComboBox::setStyle(Qt::PenCapStyle style)
{
    switch (currentIndex())
    {
    case Qt::PenCapStyle::FlatCap:
        setCurrentIndex(0);
        break;
    case Qt::PenCapStyle::SquareCap:
        setCurrentIndex(1);
        break;
    case Qt::PenCapStyle::RoundCap:
        setCurrentIndex(2);
        break;
    }
}

PointTypeComboBox::PointTypeComboBox(const Symbol *symbol, QWidget *parent)
    : QComboBox(parent)
{
    addItem(createIconForType(vrsa::common::PointSymbolType::Circle),  tr("Круг"));
    addItem(createIconForType(vrsa::common::PointSymbolType::Square),  tr("Квадрат"));
    addItem(createIconForType(vrsa::common::PointSymbolType::Triangle),tr("Треугольник"));
    addItem(createIconForType(vrsa::common::PointSymbolType::Rhombus), tr("Ромб"));
    addItem(createIconForType(vrsa::common::PointSymbolType::Cross),   tr("Крест"));
    addItem(createIconForType(vrsa::common::PointSymbolType::Plus),    tr("Плюс"));
    if (symbol)
    {
        if (symbol->type() == vrsa::common::SymbolType::SimplePointSymbol)
            setPointType(static_cast<const vrsa::graphics::SimplePointSymbol*>(symbol)->pointType);
    }
}

void PointTypeComboBox::setPointType(PointType type)
{
    switch (type)
    {
    case vrsa::common::PointSymbolType::Circle:
        setCurrentIndex(0);
        break;
    case vrsa::common::PointSymbolType::Square:
        setCurrentIndex(1);
        break;
    case vrsa::common::PointSymbolType::Triangle:
        setCurrentIndex(2);
        break;
    case vrsa::common::PointSymbolType::Rhombus:
        setCurrentIndex(3);
        break;
    case vrsa::common::PointSymbolType::Cross:
        setCurrentIndex(4);
        break;
    case vrsa::common::PointSymbolType::Plus:
        setCurrentIndex(5);
        break;
    }
}

PointTypeComboBox::PointType PointTypeComboBox::getPointType() const
{
    switch (currentIndex())
    {
    case 0:
        return vrsa::common::PointSymbolType::Circle;
    case 1:
        return vrsa::common::PointSymbolType::Square;
    case 2:
        return vrsa::common::PointSymbolType::Triangle;
    case 3:
        return vrsa::common::PointSymbolType::Rhombus;
    case 4:
        return vrsa::common::PointSymbolType::Cross;
    case 5:
        return vrsa::common::PointSymbolType::Plus;
    default:
        return vrsa::common::PointSymbolType::Circle;
    }
}

QIcon PointTypeComboBox::createIconForType(PointType type) const
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);

    switch (type)
    {
    case vrsa::common::PointSymbolType::Circle:
        painter.drawEllipse(7, 7, 10, 10);
        break;
    case vrsa::common::PointSymbolType::Square:
        painter.drawRect(7, 7, 10, 10);
        break;
    case vrsa::common::PointSymbolType::Triangle:
    {
        double centerX = 12;
        double centerY = 12;
        double size = 10;
        QPolygonF triangle;
        triangle << QPointF(centerX, centerY - size/2)           // верх
                 << QPointF(centerX - size/2, centerY + size/4)  // левый низ
                 << QPointF(centerX + size/2, centerY + size/4); // правый низ
        painter.drawPolygon(triangle);
        break;
    }
    case vrsa::common::PointSymbolType::Rhombus:
    {
        double centerX = 12;
        double centerY = 12;
        double size = 10;
        QPolygonF rhombus;
        rhombus << QPointF(centerX, centerY - size/2)            // верх
                << QPointF(centerX + size/2, centerY)            // право
                << QPointF(centerX, centerY + size/2)            // низ
                << QPointF(centerX - size/2, centerY);           // лево
        painter.drawPolygon(rhombus);
        break;
    }
    case vrsa::common::PointSymbolType::Cross:
    {
        double centerX = 12;
        double centerY = 12;
        double halfSize = 5;
        painter.drawLine(centerX - halfSize, centerY - halfSize,
                         centerX + halfSize, centerY + halfSize);
        painter.drawLine(centerX + halfSize, centerY - halfSize,
                         centerX - halfSize, centerY + halfSize);
        break;
    }
    case vrsa::common::PointSymbolType::Plus:
    {
        double centerX = 12;
        double centerY = 12;
        double halfSize = 5;
        painter.drawLine(centerX, centerY - halfSize,
                         centerX, centerY + halfSize);
        painter.drawLine(centerX - halfSize, centerY,
                         centerX + halfSize, centerY);
        break;
    }
    default:
        break;
    }
    return QIcon(pixmap);
}
