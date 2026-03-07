#ifndef SINGLESELECTIONTOOL_H
#define SINGLESELECTIONTOOL_H
#include "selectiontool.h"

namespace vrsa
{
namespace graphics
{
class FeatureGraphicsItem;
}
namespace tools
{


class SingleSelectionTool: public SelectionTool
{
public:
    SingleSelectionTool(graphics::MapScene* scene, QObject* parent = nullptr)
        : SelectionTool(scene, parent),
          mCurrentSelectedFeatureItem{nullptr} {};

    ~SingleSelectionTool() override;

    // MapTool interface
public:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    QString name() const override { return tr("Инструмент выделения одного объекта"); };
    common::MapToolType type() const override { return common::MapToolType::SingleSelectionTool; };

public:
    void selectVectorFeature(graphics::FeatureGraphicsItem* featureItem);

private:
    graphics::FeatureGraphicsItem* mCurrentSelectedFeatureItem;
};
}
}

#endif // SINGLESELECTIONTOOL_H
