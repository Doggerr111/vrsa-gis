#include "symbolrenderer.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include "graphics/symbols/simplepointsymbol.h"
#include "graphics/symbols/simplelinesymbol.h"
#include "graphics/symbols/simplepolygonsymbol.h"
#include "graphics/symbols/layersymbol.h"
#include <QIcon>
#include <QGraphicsScene>
vrsa::graphics::SymbolRenderer::SymbolRenderer(const QSize &iconSize)
    : mIconSize(iconSize),
      mItems{}
{

}

vrsa::graphics::SymbolRenderer::SymbolRenderer(QGraphicsScene *scene, const QSize &iconSize)
    : mIconSize(iconSize),
      mScene(scene),
      mItems{}
{

}

QIcon vrsa::graphics::SymbolRenderer::renderSymbolToPixmap(const Symbol *symbol, const QSize &size, Qt::AspectRatioMode mode)
{
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 1. Получаем boundingRect символа (без временного painter'а!)
    QRectF bounds = getSymbolBounds(symbol);

    // Если bounds пустой, используем тестовую геометрию
    if (bounds.isEmpty()) {
        bounds = createTestGeometryForIcon(symbol).boundingRect();
    }

    // 2. Добавляем небольшой отступ (чтобы символ не упирался в края)
    bounds.adjust(-2, -2, 2, 2);

    // 3. Рассчитываем масштаб для заполнения 80% иконки
    double targetW = size.width() * 0.8;
    double targetH = size.height() * 0.8;
    double scaleX = targetW / bounds.width();
    double scaleY = targetH / bounds.height();
    double scale = qMin(scaleX, scaleY);

    // 4. Центрируем и применяем масштаб
    painter.translate(size.width() / 2, size.height() / 2);
    painter.scale(scale, scale);
    painter.translate(-bounds.center());

    // 5. Рендерим символ
    setPainter(symbol, &painter);

    return QIcon(pixmap);

}

QGraphicsItem* vrsa::graphics::SymbolRenderer::renderSymbolToGraphicsItem(const Symbol *symbol)
{
    QPainterPath path = QPainterPath();
    for (const auto& it: mItems)
    {
        mScene->removeItem(it.get());
    }
    mItems.clear();
    renderToPathItem(symbol, &path);
    for (const auto& it: mItems)
    {
        mScene->addItem(it.get());
    }


}


void vrsa::graphics::SymbolRenderer::render(const Symbol *symbol, QPainter *painter, QGraphicsPathItem* item, RenderTarget target)
{
//    if (symbol->canHaveChildren())
//    {
//        for (int i=0; i<symbol->childCount(); i++)
//        {
//            render(symbol->child(i), painter, item, target);
//        }
//        return;
//    }
//    switch (target)
//    {
//    case Preview:
//    {
//        QPainterPath path = createTestGeometry(symbol, Preview);
//        switch (symbol->type())
//        {
//        case common::SymbolType::SimplePointSymbol:
//        {
//            painter->save();
//            auto pointS = static_cast<const SimplePointSymbol*>(symbol);
//            painter->setPen(pointS->pen());
//            painter->setBrush(pointS->brush());
//            painter->setOpacity(pointS->opacity);
//            painter->rotate(pointS->rotation);
//            painter->translate(pointS->xOffset, pointS->yOffset);
//            painter->drawPath(path);
//            painter->restore();
//            break;
//        }
//        default:
//            break;
//        }
//        break;
//    }
//    case GraphicsView:
//    {

//        createTestGeometry(symbol, GraphicsView, &pathItem);
//        switch (symbol->type())
//        {
//        case common::SymbolType::SimplePointSymbol:
//        {
//            auto pointS = static_cast<const SimplePointSymbol*>(symbol);
//            item->setPen(pointS->pen());
//            item->setBrush(pointS->brush());
//            //painter->setOpacity(pointS->opacity);
//            item->setPos(pointS->xOffset, pointS->yOffset);
//            item->setPath(pathItem);
//            break;
//        }
//        default:
//            break;
//        }
//        break;
//    }
//    }

}

void vrsa::graphics::SymbolRenderer::setPainter(const Symbol *symbol, QPainter *painter)
{
    if (symbol->canHaveChildren())
    {
        for (int i=0; i<symbol->childCount(); i++)
        {
            setPainter(symbol->child(i), painter);
        }
        return;
    }

    QPainterPath path = createTestGeometryForIcon(symbol);
    switch (symbol->type())
    {
    case common::SymbolType::SimplePointSymbol:
    {
        auto pointS = static_cast<const SimplePointSymbol*>(symbol);
        painter->save();
        painter->setPen(pointS->pen());
        painter->setBrush(pointS->brush());
        painter->setOpacity(pointS->opacity);
        painter->rotate(pointS->rotation);
        painter->translate(pointS->xOffset, pointS->yOffset);
        painter->drawPath(path);
        painter->restore();
        break;
    }
    case common::SymbolType::SimpleLineSymbol:
    {
        auto lineS = static_cast<const SimpleLineSymbol*>(symbol);
        painter->save();
        painter->setPen(lineS->pen());
        //painter->setBrush(pointS->brush());
        painter->setOpacity(lineS->opacity);
        //painter->rotate(pointS->rotation);
        painter->translate(lineS->xOffset, lineS->yOffset);
        painter->drawPath(path);
        painter->restore();
        break;
    }
    case common::SymbolType::SimplePolygonSymbol:
    {
        auto polyS = static_cast<const SimplePolygonSymbol*>(symbol);
        painter->save();
        painter->setPen(polyS->pen());
        painter->setBrush(polyS->brush());
        painter->setOpacity(polyS->opacity);
        //painter->rotate(polyS->rotation);
        painter->translate(polyS->xOffset, polyS->yOffset);
        painter->drawPath(path);
        painter->restore();
        break;
    }
    default:
        break;
    }


}

void vrsa::graphics::SymbolRenderer::renderToPathItem(const Symbol *symbol, QPainterPath *path)
{
    if (symbol->canHaveChildren())
    {
        for (int i=0; i<symbol->childCount(); i++)
        {
            renderToPathItem(symbol->child(i), path);
        }
        return;
    }
    QPainterPath path1 = QPainterPath();
    fillTestGeometryForItem(symbol, &path1);
    switch (symbol->type())
    {
    case common::SymbolType::SimplePointSymbol:
    {
        auto pointS = static_cast<const SimplePointSymbol*>(symbol);
        auto item = std::make_unique<QGraphicsPathItem>();
        item->setPen(pointS->pen());
        item->setBrush(pointS->brush());
        item->setOpacity(pointS->opacity);
        item->setRotation(pointS->rotation);
        item->setPos(pointS->xOffset, pointS->yOffset);
        item->setPath(path1);
        mItems.push_back(std::move(item));
        break;
    }
    case common::SymbolType::SimpleLineSymbol:
    {
        auto lineS = static_cast<const SimpleLineSymbol*>(symbol);
        auto item = std::make_unique<QGraphicsPathItem>();
        item->setPen(lineS->pen());
        item->setOpacity(lineS->opacity);
        item->setPos(lineS->xOffset, lineS->yOffset);
        item->setPath(path1);
        mItems.push_back(std::move(item));
        break;
    }
    case common::SymbolType::SimplePolygonSymbol:
    {
        auto polyS = static_cast<const SimplePolygonSymbol*>(symbol);
        auto item = std::make_unique<QGraphicsPathItem>();
        item->setPen(polyS->pen());
        item->setBrush(polyS->brush());
        item->setOpacity(polyS->opacity);
        item->setPos(polyS->xOffset, polyS->yOffset);
        item->setPath(path1);
        mItems.push_back(std::move(item));
        break;
    }
    default:
        break;
    }
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
        return targetSymbolSize / symbolSize;
    }
    default:
        return 1.0;

    }
}

QPainterPath vrsa::graphics::SymbolRenderer::createTestGeometryForIcon(const Symbol *symbol)
{
    QPainterPath path;
    if (symbol->canHaveChildren())
        return{};
    switch (symbol->type())
    {
    case common::SymbolType::SimplePointSymbol:
    {
        //TODO ADD point TYpe switch case
        auto pointS = static_cast<const SimplePointSymbol*>(symbol);
        switch (pointS->pointType)
        {
        case common::PointSymbolType::Circle:
        {
            double size = pointS->getPointSize()/2;
            path.addEllipse(QPointF(0,0), size/2, size/2);
            break;
        }
        case common::PointSymbolType::Square:
        {
            double size = pointS->getPointSize()/2;
            //path.addEllipse(QPointF(0,0), size/2, size/2);
            path.addRect(0-size/2, 0-size/2, size, size);
            break;
        }
        case common::PointSymbolType::Triangle:
        {
            double centerX = 0;
            double centerY = 0;
            double size = pointS->getPointSize()/2;
            QPolygonF triangle;
            triangle << QPointF(centerX, centerY - size/2)           // верх
                     << QPointF(centerX - size/2, centerY + size/4)  // левый низ
                     << QPointF(centerX + size/2, centerY + size/4); // правый низ
            path.moveTo(QPointF(centerX, centerY - size/2));
            path.lineTo(QPointF(centerX - size/2, centerY + size/4));
            path.lineTo(QPointF(centerX + size/2, centerY + size/4));
            path.closeSubpath();
            break;
        }
        case common::PointSymbolType::Rhombus:
        {
            double centerX = 0;
            double centerY = 0;
            double size = pointS->getPointSize()/2;
            auto p1 = QPointF(centerX, centerY - size/2);         // верх
            auto p2 = QPointF(centerX + size/2, centerY);         // право
            auto p3 = QPointF(centerX, centerY + size/2);         // низ
            auto p4 = QPointF(centerX - size/2, centerY);         // лево
            path.moveTo(p1);
            path.lineTo(p2);
            path.lineTo(p3);
            path.lineTo(p4);
            path.closeSubpath();
            break;
        }
        }
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
        break;
    }
    default:
        break;
    }

    return path;
}

void vrsa::graphics::SymbolRenderer::fillTestGeometryForItem(const Symbol *symbol, QPainterPath *path)
{
    if (!symbol || !path)
        return;
    if (symbol->canHaveChildren())
        return;
    switch (symbol->type())
    {
    case common::SymbolType::SimplePointSymbol:
    {
        //TODO ADD point TYpe switch case
        auto pointS = static_cast<const SimplePointSymbol*>(symbol);
        switch (pointS->pointType)
        {
        case common::PointSymbolType::Circle:
        {
            double size = pointS->getPointSize();
            path->addEllipse(QPointF(0,0), size/2, size/2);
            break;
        }
        case common::PointSymbolType::Square:
        {
            double size = pointS->getPointSize();
            //path.addEllipse(QPointF(0,0), size/2, size/2);
            path->addRect(0-size/2, 0-size/2, size, size);
            break;
        }
        case common::PointSymbolType::Triangle:
        {
            double centerX = 0;
            double centerY = 0;
            double size = pointS->getPointSize();
            path->moveTo(QPointF(centerX, centerY - size/2));
            path->lineTo(QPointF(centerX - size/2, centerY + size/4));
            path->lineTo(QPointF(centerX + size/2, centerY + size/4));
            path->closeSubpath();
            break;
        }
        case common::PointSymbolType::Rhombus:
        {
            double centerX = 0;
            double centerY = 0;
            double size = pointS->getPointSize();
            auto p1 = QPointF(centerX, centerY - size/2);         // верх
            auto p2 = QPointF(centerX + size/2, centerY);         // право
            auto p3 = QPointF(centerX, centerY + size/2);         // низ
            auto p4 = QPointF(centerX - size/2, centerY);         // лево
            path->moveTo(p1);
            path->lineTo(p2);
            path->lineTo(p3);
            path->lineTo(p4);
            path->closeSubpath();
            break;
        }

        }
        break;
    }
    case common::SymbolType::SimpleLineSymbol:
    {
        path->moveTo(-10, 0);
        path->lineTo(10, 0);
        break;
    }
    case common::SymbolType::SimplePolygonSymbol:
    {
        path->addRect((QRectF(-10, -10, 20, 20)));
        break;
    }
    default:
        break;
    }
}

QRectF vrsa::graphics::SymbolRenderer::getSymbolBounds(const Symbol *symbol)
{
    QRectF bounds;

    if (symbol->canHaveChildren())
    {
        for (int i = 0; i < symbol->childCount(); ++i)
        {
            bounds = bounds.united(getSymbolBounds(symbol->child(i)));
        }
    }
    else
    {
        bounds = createTestGeometryForIcon(symbol).boundingRect();
        double penWidth = 0.0;
        switch (symbol->type())
        {
        case common::SymbolType::SimplePointSymbol:
        {
            auto pointS = static_cast<const SimplePointSymbol*>(symbol);
            penWidth = pointS->pen().widthF();
//            qDebug()<<"Толщина пера точки в пикелях:" + QString::number(penWidth);
//            qDebug()<<"Толщина пера точки в мм:" + QString::number(pointS->borderWidth);
            break;
        }
        case common::SymbolType::SimpleLineSymbol:
        {
            auto lineS = static_cast<const SimpleLineSymbol*>(symbol);
            penWidth = lineS->borderWidth;
            break;
        }
        case common::SymbolType::SimplePolygonSymbol:
        {
            auto polyS = static_cast<const SimplePolygonSymbol*>(symbol);
            penWidth = polyS->borderWidth;
            break;
        }
        default:
            break;
        }
        double margin = (penWidth / 2.0) + 1.0;
        bounds.adjust(-margin, -margin, margin, margin);
    }

    return bounds;
}

void vrsa::graphics::SymbolRenderer::renderToIcon(Symbol *symbol, QIcon &icon)
{
    if (symbol->canHaveChildren())
        static_cast<vrsa::graphics::LayerSymbol*>(symbol)->reverseChildren();
    icon = renderSymbolToPixmap(symbol, mIconSize);
    if (symbol->canHaveChildren())
        static_cast<vrsa::graphics::LayerSymbol*>(symbol)->reverseChildren();
}

void vrsa::graphics::SymbolRenderer::renderToItem(Symbol *symbol)
{
    if (symbol->canHaveChildren())
        static_cast<vrsa::graphics::LayerSymbol*>(symbol)->reverseChildren();
    renderSymbolToGraphicsItem(symbol);
    if (symbol->canHaveChildren())
        static_cast<vrsa::graphics::LayerSymbol*>(symbol)->reverseChildren();
}
