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
    FeatureGraphicsItem(Feature *feature);
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
    const inline OGRGeometry* getFeatureGeometry() const
    {
        if (mFeature)
            return mFeature->getOGRGeometry();
        return nullptr;
    };
    const inline common::GeometryType getFeatureGeometryType() const
    {
        if (mFeature)
            return mFeature->getGeometryType();
        return common::GeometryType::Unknown;
    };

public slots:
    void setVisible(bool);
    void onZValueChanged(int zValue);
    //пока что слот используется именно при обработке сигнала с "резиновых" линий
    void onGeometryChanged(const geometry::Geometry& geometry);
    void onFeatureStyleChanged(VectorFeatureStyle*);
    void onVectorFeatureGeometryChanged();

private:
    Renderer mRenderer;
    Feature* mFeature;
    double mWidgetScale = 1.0;
    bool mIsSelected;
};


}
}
#endif // GEOGRAPHICSITEM_H
