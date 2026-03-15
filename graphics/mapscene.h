#ifndef LIPMAPSCENE_H
#define LIPMAPSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include "graphics/featuregraphicsitem.h"
#include "graphics/temporarygraphicsitem.h"
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
    void addTemporaryItem(std::unique_ptr<TemporaryGraphicsItem> item);
    void removeTemporaryItems();
    //TODO ADDVECTORDATASET

    inline double getMapScale() const noexcept { return mMapScale; };
    inline double getMapHolderScale() const noexcept { return mMapHolderScale; };
    inline void setMapHolderScale(double scale) noexcept { mMapHolderScale = scale; };
    inline void setMapScale(double scale) noexcept { mMapScale = scale; };

    void setMapTool(std::unique_ptr<tools::MapTool> tool);
    void deselectCurrentMapTool();
    void setViewCursor(QCursor cursor);

    void setPanningEnable(bool enable) noexcept { mPanningForViewEnabled = enable; }


    FeatureGraphicsItem* asFeatureItem(QGraphicsItem* item) const {
        return dynamic_cast<FeatureGraphicsItem*>(item);
    }
    TemporaryGraphicsItem* asTemporaryItem(QGraphicsItem* item) const {
        return dynamic_cast<TemporaryGraphicsItem*>(item);
    }

    // QGraphicsScene interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void mouseMoved(const QPointF&);
    void toolChanged(tools::MapTool*);
    void panningRequested(bool enable);
public slots:
    void onMapHolderScaleChanged(int mapScale, double widgetScale);

    void onVectorLayerFeatureAdded(vector::VectorFeature*);
    void onVectorLayerFeatureRemoved(int64_t fid);

    void onNewFeatureGraphicsItemCreated(std::unique_ptr<graphics::FeatureGraphicsItem>& item);
    void onFeatureGraphicsItemCreated(FeatureGraphicsItem* item); //from proj manager;

    void onMapHolderMousePressed(QMouseEvent* event);

private:
    double mMapScale;
    double mMapHolderScale;
    double mPanningForViewEnabled;
    std::vector<std::unique_ptr<graphics::FeatureGraphicsItem>> mFeatures;
    std::vector<std::unique_ptr<graphics::TemporaryGraphicsItem>> mTempItems;

    std::unique_ptr<tools::MapTool> mCurrentMapTool;

    static constexpr double X_MIN = -20037508.34;
    static constexpr double Y_MAX = 20037508.34;
    static constexpr double WIDTH = 40075016.68;
    static constexpr double HEIGHT = 20037508.34;

};

}
}

#endif // LIPMAPSCENE_H
