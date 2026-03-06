#ifndef DIGITIZINGMANAGER_H
#define DIGITIZINGMANAGER_H

#include <QObject>
#include "vector/vectorlayer.h"
#include "tools/pointdigitizingtool.h"
#include "tools/linedigitizingtool.h"
#include "graphics/featuregraphicsitem.h"
class DigitizingManager : public QObject
{
    Q_OBJECT
public:
    explicit DigitizingManager(QObject *parent = nullptr);

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

#endif // DIGITIZINGMANAGER_H
