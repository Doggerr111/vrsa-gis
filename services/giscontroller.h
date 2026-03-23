#ifndef GISCONTROLLER_H
#define GISCONTROLLER_H

#include <QObject>
#include "tools/maptool.h"
#include "common/layercreationparams.h"
#include "vectorlayertools/vectorlayercreator.h"
#include "services/projectmanager.h"
#include "graphics/symbols/symbolrenderer.h"
#include "spatialref/spatialreference.h"

class QLabel;
class QPushButton;
class QToolButton;
class QButtonGroup;
class QLineEdit;
class QComboBox;
class QStackedWidget;
class QTabWidget;
class QStatusBar;
class MapHolder;
class QTreeWidget;
class QTreeWidgetItem;
class TreeWidget;
class QDragEvent;
class QDragMoveEvent;
class QDropEvent;
class MeasurementForm;
namespace vrsa
{
namespace common{
enum class GeometryType: int;
enum class MapToolType: int;
}
namespace vector{
class VectorLayerCreator;
class VectorLayer;
}
namespace gdalwrapper
{
class Dataset;
}
namespace graphics {
class SymbolRenderer;
class MapScene;
class FeatureGraphicsItem;
class Symbol;
}
namespace services
{
class ProjectManager;
struct ViewComponents
{
    //view
    MapHolder* mapView = nullptr;
    //map tools
    QPushButton* addFeatureBtn = nullptr;
    QPushButton* singleSelectionBtn = nullptr;
    QPushButton* rectSeletionBtn = nullptr;
    QPushButton* geometryEditBtn = nullptr;
    QPushButton* rulerBtn = nullptr;
    QButtonGroup* mapToolsGrp = nullptr;
    // common
    QLineEdit* coordEdit = nullptr;
    QLineEdit* scaleEdit = nullptr;
    QComboBox* crsCombo = nullptr;
    QToolButton* lodBtn = nullptr;
    QStatusBar* statusBar = nullptr;
    QLabel* activeLayerLabel = nullptr;

    QTabWidget* leftTab = nullptr; //и дочерние виджеты
    TreeWidget* mainLegendTree = nullptr;

    QTabWidget* rightTab = nullptr; //и дочерние виджеты
    QTreeWidget* featureSelectionTree = nullptr;
    TreeWidget* postGisTree = nullptr;
    //actions vector
    QAction* actionOpenLayer = nullptr;
    QAction* actionCreateLayer = nullptr;
    QAction* actionCreatePointLayer = nullptr;
    QAction* actionCreateLineLayer = nullptr;
    QAction* actionCreatePolygonLayer = nullptr;
    //actions geoproccessing
    QAction* actionCreateBuffer = nullptr;
    QAction* actionCreateTriangulation = nullptr;
    QAction* actionCreateVoronoiDiag = nullptr;
    //geoproccesing (overlay)
    QAction* actionCreateIntersection = nullptr;
    QAction* actionCreateUnion = nullptr;
    QAction* actionCreateDifference = nullptr;
    QAction* actionCreateSymDifference = nullptr;

    //web-map services
    QAction* actionWMSConnection = nullptr;
    QAction* actionXYZConnection = nullptr;

    QAction* actionPostGisConnection = nullptr;


    //===============INTERNAL===================
    MeasurementForm* measurementForm = nullptr;

    bool isValid(std::string& errorMsg) const noexcept
    {
        if (!mapView) { errorMsg = "mapView is null"; return false; }
        if (!addFeatureBtn) {  errorMsg = "addFeatureBtn is null"; return false; }
        if (!singleSelectionBtn) {  errorMsg = "singleSelectionBtn is null"; return false; }
        if (!rectSeletionBtn) {  errorMsg = "rectSeletionBtn is null"; return false; }
        if (!geometryEditBtn) {  errorMsg = "geometryEditBtn is null"; return false; }
        if (!rulerBtn) { errorMsg = "rulerBtn is null"; return false; }
        if (!mapToolsGrp) {  errorMsg = "mapToolsGrp is null"; return false; }
        if (!coordEdit) {  errorMsg = "coordEdit is null"; return false; }
        if (!scaleEdit) {  errorMsg = "scaleEdit is null"; return false; }
        if (!crsCombo) {  errorMsg = "crsCombo is null"; return false; }
        if (!lodBtn) {errorMsg = "lodBtn is null"; return false; }
        if (!statusBar) {  errorMsg = "statusBar is null"; return false; }
        if (!leftTab) {  errorMsg = "leftTab is null"; return false; }
        if (!rightTab) {  errorMsg = "rightTab is null"; return false; }
        if (!actionOpenLayer) {  errorMsg = "actionOpenLayer is null"; return false; }
        if (!actionCreateLayer) {  errorMsg = "actionCreateLayer is null"; return false; }
        if (!actionCreatePointLayer) {  errorMsg = "actionCreatePointLayer is null"; return false; }
        if (!actionCreateLineLayer) {  errorMsg = "actionCreateLineLayer is null"; return false; }
        if (!actionCreatePolygonLayer) {  errorMsg = "actionCreatePolygonLayer is null"; return false; }
        if (!mainLegendTree) { errorMsg = "MainLegendTree is null"; return false; }
        if (!featureSelectionTree) { errorMsg = "FeatureSelectionTree is null"; return false; }
        if (!activeLayerLabel) { errorMsg = "ActiveLayerLaber is null"; return false; }
        if (!actionPostGisConnection) { errorMsg = "actionPostGisConnection is null"; return false; }
        if (!postGisTree) { errorMsg = "postGisTree is null"; return false; }
        if (!actionWMSConnection) { errorMsg = "action wms connections is null"; return false; }
        if (!actionXYZConnection) { errorMsg = "action xyz connection is null"; return false; }
        if (!actionCreateBuffer) { errorMsg = "action create buffer is null"; return false; }
        if (!actionCreateTriangulation) { errorMsg = "action create triangulations is null"; return false; }
        if (!actionCreateVoronoiDiag) { errorMsg = "action create voronoi is null"; return false; }
        if (!actionCreateIntersection) { errorMsg = "action intersection is null"; return false; }
        if (!actionCreateUnion) { errorMsg = "action create union is null"; return false; }
        if (!actionCreateDifference) { errorMsg = "action create difference is null"; return false; }
        if (!actionCreateSymDifference) { errorMsg = "action create sym difference is null"; return false; }

        return true;
    }
};

class GISController : public QObject
{
    Q_OBJECT
public:
    explicit GISController(QObject *parent = nullptr);
    void ApplyCRS(std::string name);
    bool isCurrentCRSGeographic() const;
    inline graphics::MapScene* getScene() const noexcept { return mMapScene; };
    void setupViewComponents(const ViewComponents& comp);

private:
    ViewComponents mComps;
    graphics::MapScene* mMapScene;
    spatialref::SpatialReference mProjCrs;
    //vrsa::gdalwrapper::SpatialReference mProjCrs;
    const int DATA_COLUMN = 0; //для treewidget

private:
    std::unique_ptr<graphics::SymbolRenderer> mRenderer;
    std::unique_ptr<vector::VectorLayerCreator> mVectorCreator;
    std::unique_ptr<services::ProjectManager> mProjectManager;

private:
    QIcon getIconForDigitizingToolBtn(common::GeometryType type); //например для изменения иконки оцифровки в ui
    QIcon getIconForGeometryType(common::GeometryType type);
    void addMapTool(common::MapToolType type, vector::VectorLayer *layer = nullptr);
    void removeMapTool();
    void startDigitizing();
    void syncZOrderWithTree() const;
    void setup();
    void setupViewAndIntitizlizeScene();
    void setupLegendTreeWidgets();

private slots:
    //слоты для работы с основной легендой
    void onLayerTreeDataChanged      (QTreeWidgetItem*, int);
    void onLayerTreeItemDoubleClicked(QTreeWidgetItem*, int);
    void onItemDragRequested         (QDragMoveEvent* event, bool* accepted);
    void onItemDropped               (QDropEvent* event, bool* accepted);
    void showContextMenu   (const QPoint& point);
    //слоты для работы с легендой PostGIS
    void showContextMenuPG (const QPoint& point);
    //обработка правого клика на tool btn LOD
    void showContextMenuLOD(const QPoint& point);
    //обработка кликов с кнопок инструменты карты
    void onSingleSelectionToolClicked(bool checked);
    void onRectSelectionToolClicked  (bool checked);
    void onGeometryEditToolClicked   (bool checked);
    void onDigitizingToolClicked     (bool checked);
    void onRulerToolClicked          (bool checked);
    //crs cbox
    void onCRSComboBoxIndexChanged   (int index);

    //обработка всех событий с инструментов карты
    void onToolEvent(tools::MapTool::ToolEventType type, const QVariant& data); //вся информация с инструментов
    //специфичные методы для обработки данных с инструментов
    void handleFeatureSelected         (graphics::FeatureGraphicsItem* item, bool shouldClearTree);
    void handleMultipleFeaturesSelected(const std::vector<graphics::FeatureGraphicsItem *> &items);

    void onMapHolderScaleChanged  (int mapScale, double widgetScale);
    void onMouseCoordinatesChanged(const QPointF &p);

    //для обновления ui
    void onActiveLayerChanged(const QIcon&icon); //принимаем сигнал от контроллера
    void onActiveLayerChanged(const QString& name); //принимаем сигнал от контроллера

    //слоты с project manager
    void onDatasetAdded(gdalwrapper::Dataset* dS);
    void onPostGisDatasetAdded(gdalwrapper::Dataset* dS);
    void onVectorLayerAdded(vector::VectorLayer* layer);

    //новые слоты для обработки действий с главного окна ui
    void onOpenLayerActionTriggered();
    void onCreateLayerActionTriggered();
    void onCreatePointLayerActionTriggered();
    void onCreateLineLayerActionTriggered();
    void onCreatePolygonLayerActionTriggered();


    void onCreateBufferActionTriggered();
    void onCreateTriangulationActionTriggered();
    void onCreateVoronoiActionTriggered();

    void onCreateIntersectionActionTriggered();
    void onCreateUnionActionTriggered();
    void onCreateDifferenceActionTriggered();
    void onCreateSymDifferenceActionTriggered();


    //services actions
    void onWMSConnectionTriggered();
    void onXYZConnectionTriggered();
    //BD action
    void onPostGisConnectionActionTriggered();

    //обработка сигналов с диалоговых окон
    void onVectorLayerCreationAccepted(const common::LayerDefinition &layerDef); //vectorlayercreationform

signals:
    void datasetAdded(std::string src);
    void projectCRSChanged(spatialref::SpatialReference &ref);
    void updateLegendIconsRequired(graphics::Symbol* symbol, QIcon &icon);

    void activeLayerChanged(vector::VectorLayer*);
    void activeLayerChanged(const QIcon&); //для изменения иконки для инструментов работы с активным слоем
    void activeLayerChanged(const QString&); //для изменения лейбла активного слоя

    void vectorLayerCreationRequested(const common::LayerDefinition &layerDef);
    void datasetReadingRequested     (const std::string& src);

};
}
}

#endif // GISCONTROLLER_H
