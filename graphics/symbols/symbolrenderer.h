#ifndef SYMBOLRENDERER_H
#define SYMBOLRENDERER_H



#include "calculations/unitconverter.h"
#include <QPixmap>
//add others



class QGraphicsItem;
class QGraphicsPathItem;
class QGraphicsScene;

namespace vrsa
{
namespace graphics
{

class Symbol;

enum RenderTarget
{
    GraphicsView, //for QGraphicsView (real size)
    Preview // icons for treewidget
};

//класс для рендера символа в иконку для StylingForm!! Весь рендер на карте производится при помощи классов drawingPolicy!!

class SymbolRenderer: public QObject
{
    Q_OBJECT
public:
    SymbolRenderer(const QSize& iconSize = QSize(24,24));
    SymbolRenderer(QGraphicsScene* scene, const QSize& iconSize = QSize(24,24));

    //
    static QIcon renderSymbolToPixmap(const Symbol* symbol,
                                const QSize& size = QSize(24, 24),
                                Qt::AspectRatioMode mode = Qt::KeepAspectRatio);
    QGraphicsItem *renderSymbolToGraphicsItem(const Symbol* symbol);
    static void renderSimpleSymbol(const Symbol* symbol,
                                      QPainter* painter,
                                      const QPainterPath& geometry);


    static void render(const Symbol *symbol, QPainter* painter, QGraphicsPathItem *item, RenderTarget target);

    static void setPainter(const Symbol* symbol, QPainter* painter);
    void renderToPathItem(const Symbol* symbol, QPainterPath* path);
    static QPainterPath createTestGeometryForIcon(const Symbol* symbol);
    static void fillTestGeometryForItem(const Symbol* symbol, QPainterPath* path);

    static std::unique_ptr<QGraphicsItem> renderGraphicsItem(const Symbol* symbol);
    static double calculateScale(const Symbol* symbol, const QSize& targetSize);

    static QRectF getSymbolBounds(const Symbol *symbol);

public slots:
    void renderToIcon(Symbol* symbol, QIcon &icon);
    void renderToItem(Symbol* symbol);
private:
    QSize mIconSize;
    QGraphicsScene* mScene;
    std::vector<std::unique_ptr<QGraphicsPathItem>> mItems;


};

}
}

#endif // SYMBOLRENDERER_H
