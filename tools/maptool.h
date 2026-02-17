#ifndef MAPTOOL_H
#define MAPTOOL_H

#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QIcon>
#include "common/GisDefines.h"
#include "vector/vectorfeature.h"
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


    virtual bool canCancel() const noexcept { return false; }
    virtual void cancel() {};

    graphics::MapScene* scene() const noexcept { return mMapScene; };

signals:
    void activated();
    void deactivated();
    void cancelled();
    void statusMessage(const QString& message);
    void geometryCreated(vrsa::vector::VectorFeature::GeometryVariant);

protected:
    graphics::MapScene* mMapScene;
    QPointF mLastScenePos;
    QVector<QPointF> mPoints;
    bool mIsActive;



};

}
}
#endif // MAPTOOL_H
