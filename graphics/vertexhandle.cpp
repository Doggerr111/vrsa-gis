#include "vertexhandle.h"
#include <QGraphicsSceneMouseEvent>
vrsa::graphics::VertexHandle::VertexHandle(TemporaryGraphicsItem *parent, VertexType type)
    : TemporaryGraphicsItem(common::GeometryType::Point, TemporaryGraphicsItem::TemporaryItemRole::VertexHandle),
      mParentItem{parent},
      mCurrentState{VertexState::Normal},
      mType{type}
{
    setZValue(common::MAX_Z_VALUE+2);
    setAcceptHoverEvents(true);
    QObject::connect(this, &VertexHandle::stateChanged, this, &VertexHandle::onStateChanged);
    TemporaryGraphicsItem::setupRenderer();
}

void vrsa::graphics::VertexHandle::updateGeometry()
{
    qDebug()<<"vertex handle updateGeometry() called";
    geometry::Geometry geom;
    geom.type = common::GeometryType::Point;
    geom.variant = mCurrentPos;
    TemporaryGraphicsItem::setGeometry(geom);
    emit geometryChanged(mCurrentPos);

}

void vrsa::graphics::VertexHandle::setPoint(const QPointF &point)
{
    mCurrentPos = point;
    updateGeometry();
}

void vrsa::graphics::VertexHandle::onStateChanged()
{
    qDebug()<< "VertexHandle onStateChanged() Current state:" <<mCurrentState;
    generateStyle();
    updateStyle();
    update();
}





QRectF vrsa::graphics::VertexHandle::boundingRect() const
{
    return TemporaryGraphicsItem::boundingRect();
}

void vrsa::graphics::VertexHandle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    return TemporaryGraphicsItem::paint(painter, option, widget);
}

void vrsa::graphics::VertexHandle::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    mCurrentState = VertexState::Hover;
    emit hovered();
    emit stateChanged();
}

void vrsa::graphics::VertexHandle::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{

}

void vrsa::graphics::VertexHandle::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    mCurrentState = VertexState::Normal;
    emit stateChanged();
}

void vrsa::graphics::VertexHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    mType = VertexType::Vertex;
    mCurrentState = VertexState::Pressed;
    emit clicked();
    emit clicked(event->scenePos());
    emit stateChanged();
}

void vrsa::graphics::VertexHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (mCurrentState == VertexState::Pressed)
    {
        setPoint(event->scenePos());
    }
    auto pos = event->scenePos();
    emit moved(pos);
    emit stateChanged();
}

void vrsa::graphics::VertexHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    mCurrentState = VertexState::Hover;
    emit stateChanged();
    emit released();
}


vrsa::graphics::VectorFeatureStyle *vrsa::graphics::VertexHandle::generateStyle()
{
    if (mType == VertexType::Vertex)
        mStyle = VectorFeatureStyle::createForVertexHandles(mCurrentState);
    else if (mType == VertexType::MiddlePoint)
        mStyle = VectorFeatureStyle::createForVertexMiddlePoints(mCurrentState);
    return mStyle.get();
}
