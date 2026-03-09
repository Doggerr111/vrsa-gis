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
    void addVertex(const QPointF& point, bool isMidPoint = false);
    void updateGeometry();
    void clearVertexes();
    void hideMiddleVertexes();
    geometry::Geometry getTargetOriginalGeometry() const {return mTargetOriginalGeometry; };

private:
    void buildVertexes(const std::vector<QPointF>* points) { rebuildVertexes(points, false); }
    void rebuildVertexes(const std::vector<QPointF>* points, bool initialBuildDone = true); //только для линий и полигонов !!!!
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
    void onVertexClicked();
signals:
    void rubberBandGeometryChanged(const geometry::Geometry& geometry);
    void vertexReleased(graphics::FeatureGraphicsItem*, geometry::Geometry); //for tool
};

}
}
#endif // RUBBERBANDITEM_H
