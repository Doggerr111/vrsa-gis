#ifndef EDITGEOMETRYTOOL_H
#define EDITGEOMETRYTOOL_H

#include "singleselectiontool.h"
#include <QObject>
#include <stack>

namespace vrsa
{
namespace geometry
{
class Geometry;
}
namespace graphics
{
class FeatureGraphicsItem;
class RubberBandItem;
}
namespace tools
{


class EditGeometryTool : public MapTool
{
    Q_OBJECT


    enum EditState { Selecting, Editing };
public:
    explicit EditGeometryTool(graphics::MapScene* scene, QObject *parent = nullptr);
    ~EditGeometryTool() override;

private:
    void undo();
    void redo();
    void undoAll();

private:
    //для первоначального выбора фичи с карты
    std::unique_ptr<SingleSelectionTool> mSelectionTool;
    EditState mCurrentState;
    graphics::FeatureGraphicsItem* mSelectedFeature; //то, что придет с инструмента выше
    graphics::RubberBandItem* mRubberBand;

    std::stack<std::pair<graphics::FeatureGraphicsItem*, geometry::Geometry>> mUndoStack;
    std::stack<std::pair<graphics::FeatureGraphicsItem*, geometry::Geometry>> mRedoStack;

private slots:
    void onFeatureSelected(ToolEventType type, const QVariant& data = QVariant());
    void onVertexReleased(graphics::FeatureGraphicsItem*, geometry::Geometry);


    // MapTool interface
public:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    QString name() const override { return tr("Инструмент, позволяющий изменить геометрию слоя"); };
    common::MapToolType type() const override { return common::MapToolType::EditGeometryTool; };
    void deactivate() override;
};

}
}
#endif // EDITGEOMETRYTOOL_H
