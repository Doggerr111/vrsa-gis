#ifndef MAPTOOL_H
#define MAPTOOL_H

#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QIcon>
#include "common/GisDefines.h"
#include "vector/vectorfeature.h"
#include <QCursor>
namespace vrsa {
namespace graphics {
    class MapScene;
}};

namespace vrsa
{
namespace tools
{

class MapTool : public QObject
{
    Q_OBJECT
public:
    explicit MapTool(graphics::MapScene* scene, QObject *parent = nullptr);
    virtual ~MapTool() = default;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) { Q_UNUSED(event); }
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { Q_UNUSED(event); };
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) { Q_UNUSED(event); };
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) { Q_UNUSED(event); };
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event) { Q_UNUSED(event); };

    virtual void keyPressEvent(QKeyEvent* event) { Q_UNUSED(event); };
    virtual void keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); };

    virtual QString name() const = 0;
    virtual common::MapToolType type() const = 0;
    virtual QString toolTip() const { return{}; };
    virtual QIcon icon() const { return{}; };
    virtual QCursor cursor() const { return QCursor(Qt::ArrowCursor); };


    virtual bool canCancel() const noexcept { return false; }
    virtual void cancel() {};
    virtual void deactivate() {emit deactivated();};
    graphics::MapScene* scene() const noexcept { return mMapScene; };
//    virtual void setScene(graphics::MapScene* scene) { mMapScene = scene; };


    //перечесление для создания подобия шаблонного сигнала, который будет связываться с контролером
    //GisController для обмена данными при необходимости (сигнал - toolEvent), например, для обновления ui
    enum class ToolEventType {
        FeatureSelected,
        MultipleFeaturesSelected,
        FeatureDoubleClicked,
        SelectionCleared,
        DigitizingFinished,
        CursorPositionChanged,
        SimpleLineRulerGeometryChanged
    };


signals:
    void activated();
    void deactivated();
    void cancelled();
    void statusMessage(const QString& message, int timeout_ms = 0);
    void toolEvent(ToolEventType type, const QVariant& data = QVariant());
protected:
    graphics::MapScene* mMapScene;

    //QVector<QPointF> mPoints;
    bool mIsActive;



};

}
}
#endif // MAPTOOL_H
