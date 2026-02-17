#ifndef SYMBOLRENDERER_H
#define SYMBOLRENDERER_H

#include "graphics/symbols/layerpointsymbol.h"
#include "graphics/symbols/simplepointsymbol.h"
#include "graphics/symbols/simplelinesymbol.h"
#include "graphics/symbols/simplepolygonsymbol.h"
#include "calculations/unitconverter.h"
#include <QPixmap>
//add others



class QGraphicsItem;
namespace vrsa
{
namespace graphics
{

enum RenderTarget
{
    GraphicsView, //for QGraphicsView (real size)
    Preview // icons for treewidget
};

//класс для рендера символа в иконку для StylingForm!! Весь рендер на карте производится при помощи классов drawingPolicy!!

class SymbolRenderer
{
public:
    SymbolRenderer() = default;
    static QPixmap renderSymbol(const Symbol* symbol,
                                const QSize& size = QSize(24, 24),
                                Qt::AspectRatioMode mode = Qt::KeepAspectRatio);

    static void renderSimpleSymbol(const Symbol* symbol,
                                      QPainter* painter,
                                      const QPainterPath& geometry);
    static void render(const Symbol *symbol, QPainter* painter);
    static std::unique_ptr<QGraphicsItem> renderGraphicsItem(const Symbol* symbol);

    static double calculateScale(const Symbol* symbol, const QSize& targetSize);

    static QPainterPath createTestGeometry(const Symbol* symbol, RenderTarget target);


};

}
}

#endif // SYMBOLRENDERER_H
