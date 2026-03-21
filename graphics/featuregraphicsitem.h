#ifndef GEOGRAPHICSITEM_H
#define GEOGRAPHICSITEM_H
#include <QGraphicsItem>
#include "graphics/featuregraphicsitemrenderer.h"
#include "geometry/geometry.h"
namespace vrsa
{
namespace vector{
class VectorFeature;
}


namespace graphics
{

class FeatureGraphicsItem: public QObject, public QGraphicsItem
{
    Q_OBJECT
    using Renderer = std::unique_ptr<FeatureGraphicsItemRenderer>;
    using Feature = vrsa::vector::VectorFeature;
public:
    FeatureGraphicsItem(Feature *feature);
    inline void setScale(double mapScaleDenominator, double mapHolderScaleFactor) noexcept
    {
        mWidgetScale = mapHolderScaleFactor;
        mMapScale = mapScaleDenominator;
    }
    void setSelected(const bool selected);
    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    inline Feature* getFeature() const noexcept {return mFeature; };
    const OGRGeometry* getFeatureGeometry() const;;
    const common::GeometryType getFeatureGeometryType() const;

public slots:
    void setVisible(bool);
    void onZValueChanged(int zValue);
    //пока что слот используется именно при обработке сигнала с "резиновых" линий
    void onGeometryChanged(const geometry::Geometry& geometry);
    void onFeatureStyleChanged(VectorFeatureStyle*);
    void onVectorFeatureGeometryChanged();
    void onSymbolUpdated();
    void onLodSettingsChanged(bool enabled, common::LodAlgorithmType type);

private:
    Renderer mRenderer;
    Feature* mFeature;
    double mWidgetScale = 1.0;
    double mMapScale;
    bool mIsSelected;
    bool mIsLodEnabled;
    common::LodAlgorithmType mLodAlgo;

};


}
}
#endif // GEOGRAPHICSITEM_H
