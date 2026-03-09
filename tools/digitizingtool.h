#ifndef DIGITIZINGTOOL_H
#define DIGITIZINGTOOL_H
#include "maptool.h"
#include "graphics/temporarygraphicsitem.h"

namespace vrsa
{
namespace graphics
{
class FeatureGraphicsItem;
}
namespace tools
{
class DigitizeHelper;
class DigitizingTool : public MapTool
{
    Q_OBJECT
public:
    explicit DigitizingTool(graphics::MapScene* scene, vector::VectorLayer *activeLayer, QObject *parent = nullptr);
    virtual ~DigitizingTool() = default;
protected:
    vrsa::geometry::Geometry mGeom;
    graphics::TemporaryGraphicsItem* mItem;
    std::unique_ptr<DigitizeHelper> mDigitizeManager;

signals:
    void geometryCreated(geometry::Geometry&);
    //std::vector<QPointF> mPoints;

};



class DigitizeHelper : public QObject
{
    Q_OBJECT
public:
    explicit DigitizeHelper(QObject *parent = nullptr);

    inline void setActiveLayer(vrsa::vector::VectorLayer* activeLayer) noexcept { mActiveLayer = activeLayer; };
    inline void setActiveDigitizingTool(vrsa::tools::DigitizingTool* tool) noexcept {mActiveTool = tool; }
    bool isToolCompatibleWithLayer(vrsa::common::MapToolType toolType, vrsa::common::GeometryType layerType) const noexcept;

private:
    vrsa::vector::VectorLayer* mActiveLayer;
    vrsa::tools::DigitizingTool* mActiveTool;
public slots:
    void onGeometryCreated(vrsa::geometry::Geometry &geometry);
    void onDigitizingFinished();
    void onDigitizingCanceled();

signals:
    void featureGraphicsItemCreated(std::unique_ptr<vrsa::graphics::FeatureGraphicsItem>&);
};



}
}
#endif // DIGITIZINGTOOL_H
