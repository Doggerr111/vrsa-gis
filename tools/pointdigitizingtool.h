#ifndef POINTDIGITIZINGTOOL_H
#define POINTDIGITIZINGTOOL_H

#include "digitizingtool.h"
#include "QGraphicsEllipseItem"
namespace vrsa
{
namespace tools
{

class PointDigitizingTool : public DigitizingTool
{
public:
    explicit PointDigitizingTool(graphics::MapScene* scene, QObject *parent = nullptr);
    virtual ~PointDigitizingTool() = default;
    // MapTool interface
public:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    QString name() const override;
    QString toolTip() const override;
    inline common::MapToolType type() const noexcept override { return common::MapToolType::PointDigitizingTool; };
    inline bool canCancel() const noexcept override { return true; };
    void cancel() override;

};

}
}

#endif // POINTDIGITIZINGTOOL_H
