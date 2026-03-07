#ifndef RECTSELECTIONTOOL_H
#define RECTSELECTIONTOOL_H

#include "selectiontool.h"
#include "geometry/geometry.h"
namespace vrsa
{
namespace graphics
{
class FeatureGraphicsItem;
class TemporaryGraphicsItem;
}
namespace tools
{



class RectSelectionTool : public SelectionTool
{
public:
    explicit RectSelectionTool(graphics::MapScene* scene, QObject *parent = nullptr);
    ~RectSelectionTool() override;

    // MapTool interface
public:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    QString name() const override { return tr("Инструмент выделения объектов при помощи ограничивающего прямоугольника"); };
    common::MapToolType type() const override { return common::MapToolType::RectSelectionTool; };

public:
    //для обновления  айтема
    inline void updateGeometry();
    void deselectItems();
    //void selectItems();

private:
    std::vector<graphics::FeatureGraphicsItem*> mSelectedFeatureItems;
    graphics::TemporaryGraphicsItem* mSelectRectItem;
    QPointF mClickPos;
    QPointF mCurrentPos;
    bool mIsLeftClicked;

};

}
}

#endif // RECTSELECTIONTOOL_H
