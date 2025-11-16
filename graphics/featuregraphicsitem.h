#ifndef GEOGRAPHICSITEM_H
#define GEOGRAPHICSITEM_H
#include <QGraphicsItem>
#include "graphics/drawingcontext.h"
#include "graphics/pointfeaturedrawingpolicy.h"
#include "vector/vectorfeature.h"
namespace vrsa
{
namespace graphics
{

class FeatureGraphicsItem: public QGraphicsItem
{
    using DrawingPolicy = std::unique_ptr<VectorFeatureDrawingPolicy>;
    using Feature = std::unique_ptr<vrsa::vector::VectorFeature>;
public:
    FeatureGraphicsItem(DrawingPolicy dPolicy,const Feature& feature);
    inline void setScale(int s)
    {
        mWidgetScale = s;
        //update();

    }
    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    DrawingPolicy mPolicy;
    const Feature& mFeature;
    double mWidgetScale;


};


}
}
#endif // GEOGRAPHICSITEM_H
