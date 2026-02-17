#ifndef DIGITIZINGMANAGER_H
#define DIGITIZINGMANAGER_H

#include <QObject>
#include "vector/vectorlayer.h"
#include "tools/pointdigitizingtool.h"
class DigitizingManager : public QObject
{
    Q_OBJECT
public:
    explicit DigitizingManager(QObject *parent = nullptr);

    inline void setActiveLayer(vrsa::vector::VectorLayer* activeLayer) noexcept { mActiveLayer = activeLayer; };
    inline void setActiveDigitizingTool(vrsa::tools::DigitizingTool* tool) noexcept {mActiveTool = tool; }


private:
    vrsa::vector::VectorLayer* mActiveLayer;
    vrsa::tools::DigitizingTool* mActiveTool;
public slots:
    void onGeometryCreated(vrsa::vector::VectorFeature::GeometryVariant);
    void onDigitizingFinished();
    void onDIgitizingCanceled();

signals:

};

#endif // DIGITIZINGMANAGER_H
