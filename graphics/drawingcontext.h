#ifndef DRAWINGCONTEXT_H
#define DRAWINGCONTEXT_H
#include "graphics/vectorfeaturestyle.h"
#include <QStyleOptionGraphicsItem>
#include "gdal/gdalresourcehandles.h"
namespace vrsa
{
namespace graphics
{


struct DrawingContext
{
    //from QGraphicsItem::paint()
    QPainter* painter;
    const QStyleOptionGraphicsItem* option;
    QWidget* widget;
    OGRGeometry* geom;
    double sceneScale;
    //vrsa::gdalwrapper::OgrGeometryPtr geometry;
};

}
}
#endif // DRAWINGCONTEXT_H
