#ifndef LINEDIGITIZINGTOOL_H
#define LINEDIGITIZINGTOOL_H
#include "digitizingtool.h"
namespace vrsa
{
namespace tools
{

class LineDigitizingTool : public DigitizingTool
{
    using ItemRole = graphics::TemporaryGraphicsItem::TemporaryItemRole;
public:
    explicit LineDigitizingTool(graphics::MapScene* scene, QObject *parent = nullptr);
    virtual ~LineDigitizingTool() = default;

    //для обновления  айтема
    inline void updateGeometry();
    inline void clearGeometry();
    inline std::vector<QPointF>& points()
    {
        return std::get<std::vector<QPointF>>(mGeom.variant);
    }

    inline const std::vector<QPointF>& points() const
    {
        return std::get<std::vector<QPointF>>(mGeom.variant);
    }
    // MapTool interface
public:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    QString name() const override;
    QString toolTip() const override;
    inline common::MapToolType type() const noexcept override { return common::MapToolType::LineStringDigitizingTool; };
    inline bool canCancel() const noexcept override { return true; };
    void cancel() override;

private:
    graphics::TemporaryGraphicsItem* mItem;
    std::vector<QPointF> mPoints;
    QPointF mLastScenePos;
    int mFixedCount;

};
}
}

#endif // LINEDIGITIZINGTOOL_H
