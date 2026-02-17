#ifndef LIPMAPSCENE_H
#define LIPMAPSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include "graphics/featuregraphicsitem.h"
#include "vector/vectorlayer.h"
#include "graphics/rastergraphicsitemfactory.h"
#include "raster/rasterdataset.h"
#include "tools/pointdigitizingtool.h"


namespace vrsa
{
namespace graphics
{


class MapScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MapScene(QObject *parent = nullptr);

    void addLayer(std::unique_ptr<vector::VectorLayer>& l);
    void addLayer(std::unique_ptr<raster::RasterChannel>& channel);
    void addRasterDataset(raster::RasterDataset* dS);
    //TODO ADDVECTORDATASET

    inline double getMapScale() noexcept { return mMapScale; };
    inline double getMapHolderScale() noexcept { return mMapHolderScale; };

    void setTool(std::unique_ptr<tools::MapTool> tool);


    // QGraphicsScene interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void mouseMoved(QPointF);
    void toolChanged(tools::MapTool*);
public slots:
    void onMapHolderScaleChanged(int mapScale, double widgetScale);

    void onVectorLayerFeatureAdded(vector::VectorFeature*);
    void onVectorLayerFeatureRemoved(vector::VectorFeature*);

private:
    double mMapScale;
    double mMapHolderScale;

    std::vector<std::unique_ptr<graphics::FeatureGraphicsItem>> mFeatures;

    std::unique_ptr<tools::MapTool> mCurrentMapTool;

};

}
}

#endif // LIPMAPSCENE_H
