#ifndef VERTEXHANDLE_H
#define VERTEXHANDLE_H

#include "temporarygraphicsitem.h"
#include <QObject>

namespace vrsa
{
namespace graphics
{

//не внутри класса для возможности предварительного объявления например в featurestyle
enum class VertexState: uint8_t
{
    Normal,
    Hover,
    Pressed,
    Disabled
};

enum class VertexType: uint8_t
{
    Vertex, //обычная точка по координатам оригинальной геометрии
    MiddlePoint //точка посередине для создания новых сегментов
};

//"якоря" для резинок
class VertexHandle : public TemporaryGraphicsItem
{


    Q_OBJECT
public:
    VertexHandle(TemporaryGraphicsItem* parent, VertexType type = VertexType::Vertex);
    ~VertexHandle() override = default;

    VertexState getCurrentState() const noexcept {  return mCurrentState; };
    void updateGeometry();
    void setPoint(const QPointF& point);
    QPointF getPoint() const noexcept { return mCurrentPos; };
    VertexType getType() const noexcept { return mType; };


    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    // TemporaryGraphicsItem interface
private:
    VectorFeatureStyle *generateStyle() override;

private:
    TemporaryGraphicsItem* mParentItem;
    VertexState mCurrentState;
    std::unique_ptr<VectorFeatureStyle>& mCurrentStyle = TemporaryGraphicsItem::mStyle;
    QPointF mCurrentPos;
    VertexType mType;
signals:
    void moved(const QPointF& newPos);
    void clicked(const QPointF& pos);
    void clicked();
    void released();
    void hovered();
    void stateChanged();
    void geometryChanged(const QPointF& pos);
private slots:
    void onStateChanged();

};

inline QDebug operator<<(QDebug debug, VertexState state) {
    switch (state) {
        case VertexState::Normal:
            debug << "Normal";
            break;
        case VertexState::Hover:
            debug << "Hover";
            break;
        case VertexState::Pressed:
            debug << "Pressed";
            break;
        case VertexState::Disabled:
            debug << "Disabled";
            break;
        default:
            debug << "Unknown(" << static_cast<int>(state) << ")";
            break;
    }
    return debug;
}

}
}

#endif // VERTEXHANDLE_H
