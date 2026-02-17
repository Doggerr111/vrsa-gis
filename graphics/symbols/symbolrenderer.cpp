#include "symbolrenderer.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsItem>
QPixmap vrsa::graphics::SymbolRenderer::renderSymbol(const Symbol *symbol, const QSize &size, Qt::AspectRatioMode mode)
{
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);  // Прозрачный фон

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Центрируем символ
    painter.translate(size.width() / 2, size.height() / 2);

    // Масштабируем под размер иконки
//    double scale = calculateScale(symbol, size);
//    painter.scale(scale, scale);

    // Создаем тестовую геометрию в зависимости от типа символа
    //QPainterPath testGeometry = createTestGeometry(symbol);

    render(symbol, &painter);

    return pixmap;

}

void vrsa::graphics::SymbolRenderer::renderSimpleSymbol(const Symbol *symbol, QPainter *painter, const QPainterPath &geometry)
{
//    // Получаем тип символа
//    QString symbolType = symbol->type();
//    // Применяем базовые настройки
//    painter->setPen(symbol->pen());
//    painter->setBrush(symbol->brush());

//    if (symbolType == "point") {
//        // Для точек - рисуем в зависимости от типа точки
//        QString pointType = symbol->parameter("pointType").toString("circle");
//        double size = symbol->parameter("size").toDouble(4.0);

//        if (pointType == "circle") {
//            painter->drawEllipse(QRectF(-size/2, -size/2, size, size));
//        } else if (pointType == "square") {
//            painter->drawRect(QRectF(-size/2, -size/2, size, size));
//        } else {
//            painter->drawPath(geometry);
//        }
//    } else {
//        // Для линий и полигонов - просто рисуем геометрию
//        painter->drawPath(geometry);
//    }
}

void vrsa::graphics::SymbolRenderer::render(const Symbol *symbol, QPainter *painter)
{
    if (symbol->canHaveChildren())
    {
        for (int i=0; i<symbol->childCount(); i++)
        {
            render(symbol->child(i), painter);
        }
        return;
    }

    double scale = calculateScale(symbol, QSize(24,24));
    //painter->scale(scale, scale);
    QPainterPath path = createTestGeometry(symbol, Preview);
    switch (symbol->type())
    {

    case common::SymbolType::SimplePointSymbol:
        {
            auto pointS = static_cast<const SimplePointSymbol*>(symbol);
            painter->setPen(pointS->pen());
            painter->setBrush(pointS->brush());
            //painter->setOpacity(pointS->opacity);
            painter->translate(pointS->xOffset, pointS->yOffset);
            painter->drawPath(path);
            break;
        }
    default:
        break;
    }

}

std::unique_ptr<QGraphicsItem> vrsa::graphics::SymbolRenderer::renderGraphicsItem(const Symbol *symbol)
{

}

double vrsa::graphics::SymbolRenderer::calculateScale(const Symbol *symbol, const QSize &targetSize)
{
    // Определяем размер символа
    switch (symbol->type())
    {
    case common::SymbolType::SimplePointSymbol:
    {
        auto s = static_cast<const SimplePointSymbol*>(symbol);
        double symbolSize = calculations::UnitConversion::convertToPixels(s->pointSize, s->unitType);
        double targetSymbolSize = targetSize.width() * 0.8;

        qDebug() << "    original pointSize:" << s->pointSize;  // Добавь это!
        qDebug() << "    unitType:" << static_cast<int>(s->unitType);  // И это!
        qDebug() << "    converted symbolSize:" << symbolSize;
        return targetSymbolSize / symbolSize;
    }
    default:
        break;

    }
}

QPainterPath vrsa::graphics::SymbolRenderer::createTestGeometry(const Symbol *symbol, RenderTarget target)
{
    QPainterPath path;
    if (symbol->canHaveChildren())
        return{};
    switch (target)
    {
    case Preview:
    {
        switch (symbol->type())
        {
        case common::SymbolType::SimplePointSymbol:
        {
            //TODO ADD point TYpe switch case
            auto pointS = static_cast<const SimplePointSymbol*>(symbol);
            double size = pointS->pointSize;
            path.addEllipse(QPointF(0,0), size/2, size/2);
            break;
        }
        case common::SymbolType::SimpleLineSymbol:
        {
            path.moveTo(-10, 0);
            path.lineTo(10, 0);
            break;
        }
        case common::SymbolType::SimplePolygonSymbol:
        {
            path.addRect((QRectF(-10, -10, 20, 20)));
        }
        default:
            break;
        }
    }

    }
    return path;
}
