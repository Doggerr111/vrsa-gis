#ifndef RUBBERBANDITEM_H
#define RUBBERBANDITEM_H

#include "temporarygraphicsitem.h"
#include <QObject>

namespace vrsa
{
namespace graphics
{
class FeatureGraphicsItem;
class VertexHandle;
class RubberBandItem : public TemporaryGraphicsItem
{
    Q_OBJECT
public:
    //"резинки" для разных типов геометрий (точка линия полигон)
    RubberBandItem(common::GeometryType geomType);
    ~RubberBandItem() override;
    void setTargetItem(FeatureGraphicsItem* target);
    void updateGeometry();
    void clearVertexes();
    geometry::Geometry getTargetOriginalGeometry() const {return mTargetOriginalGeometry; };
private:
    //"якоря" itemTypeRole::VertexHandle - точки
    std::vector<std::unique_ptr<VertexHandle>> mAnchorPoints;
    FeatureGraphicsItem* mTargetFeatureItem;
    geometry::Geometry mTargetOriginalGeometry;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setScale(const double scale) override;

public slots:
    void onVertexGeometryChanged(const QPointF& point);
    void onVertexReleased();
signals:
    void rubberBandGeometryChanged(const geometry::Geometry& geometry);
    void vertexReleased(graphics::FeatureGraphicsItem*, geometry::Geometry); //for tool
};

}
}
#endif // RUBBERBANDITEM_H
