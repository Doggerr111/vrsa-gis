#ifndef GEOGRAPHICSITEM_H
#define GEOGRAPHICSITEM_H
#include <QGraphicsItem>
#include "graphics/drawingcontext.h"
#include "graphics/featuregraphicsitemrenderer.h"
#include "vector/vectorfeature.h"
namespace vrsa
{
namespace graphics
{

class FeatureGraphicsItem: public QObject, public QGraphicsItem
{
    Q_OBJECT
    using Renderer = std::unique_ptr<FeatureGraphicsItemRenderer>;
    using Feature = vrsa::vector::VectorFeature;
public:
    FeatureGraphicsItem(Renderer renderer,const Feature* feature);
    inline void setScale(double s)
    {
        mWidgetScale = s;
        //update();

    }
    void setSelected(const bool selected);
    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    const inline Feature* getFeature() const noexcept {return mFeature; };

public slots:
    void setVisible(bool);
    void onZValueChanged(int zValue);

private:
    Renderer mRenderer;
    const Feature* mFeature;
    double mWidgetScale;
    bool mIsSelected;
};


}
}
#endif // GEOGRAPHICSITEM_H
