#include "vertexhandle.h"
#include <QGraphicsSceneMouseEvent>
vrsa::graphics::VertexHandle::VertexHandle(TemporaryGraphicsItem *parent)
    : TemporaryGraphicsItem(common::GeometryType::Point, TemporaryGraphicsItem::TemporaryItemRole::VertexHandle),
      mParentItem{parent},
      mCurrentState{VertexState::Normal}
{
    setZValue(common::MAX_Z_VALUE+2);
    setAcceptHoverEvents(true);
    QObject::connect(this, &VertexHandle::stateChanged, this, &VertexHandle::onStateChanged);
    TemporaryGraphicsItem::setupRenderer();
}

void vrsa::graphics::VertexHandle::updateGeometry()
{
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
    qDebug()<< mCurrentState;
    mCurrentStyle = VectorFeatureStyle::createForVertexHandles(mCurrentState);
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
    mCurrentState = VertexState::Pressed;
    auto pos = event->scenePos();
    emit clicked(pos);

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
    qDebug()<<"VertexHandle generateStyle()";
    mStyle = VectorFeatureStyle::createForVertexHandles(mCurrentState);
    return mStyle.get();
}
