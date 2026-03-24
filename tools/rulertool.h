#ifndef RULERTOOL_H
#define RULERTOOL_H
#include "maptool.h"

namespace vrsa
{
namespace graphics
{
class FeatureGraphicsItem;
class TemporaryGraphicsItem;
}
namespace tools
{



class RulerTool : public MapTool
{
public:
    explicit RulerTool(graphics::MapScene* scene, QObject *parent = nullptr);
    ~RulerTool() override;

    // MapTool interface
public:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    QString name() const override { return tr("Инструмент измерения расстояний"); };
    common::MapToolType type() const override { return common::MapToolType::RulerTool; };

public:
    inline void updateGeometry();

private:
    graphics::TemporaryGraphicsItem* mRulerItem;
    QPointF mClickPos;
    QPointF mCurrentPos;
    bool mIsLeftClicked;
};

}
}
#endif // RULERTOOL_H
