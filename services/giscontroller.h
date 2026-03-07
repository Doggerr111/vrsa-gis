#ifndef GISCONTROLLER_H
#define GISCONTROLLER_H

#include <QObject>
#include <QGraphicsView>
#include "graphics/mapscene.h"
#include "common/logger.h"
#include "services/projectmanager.h"
#include "gdal/gdalreader.h"
#include "calculations/mapcalculations.h"
#include "georef/spatialreferencelibrary.h"
#include "gdal/spatialreference.h"
#include "customWidgets/mapholder.h"
#include "customWidgets/treewidget.h"
#include <QTreeWidget>
#include <QAction>
#include <QMenu>
#include "digitizingmanager.h"
#include "graphics/symbols/symbolrenderer.h"
#include "tools/maptool.h"



namespace vrsa
{
namespace services
{


class GISController : public QObject
{
    Q_OBJECT
public:
    explicit GISController(QObject *parent = nullptr);
    void LoadDataSet(std::string& source);
    void ApplyCRS(std::string name);
    bool isCurrentCRSGeographic() const;
    inline graphics::MapScene* getScene() const noexcept
    {
        return mMapScene;
    }
    void startDigitizing() const;
    void stopDigitizing() const;
    void syncZOrderWithTree();

    QIcon getIconForGeometryType(common::GeometryType type); //например для изменения иконки оцифровки в ui
    void addMapTool(common::MapToolType type);


    //некоторые компоненты интерфейса придется хранить... надеюсь когда-нибудь перейду просто на сигналы и слоты
    void initializeScene(MapHolder* view);
    void setTreeWidget(TreeWidget* treeWidget);
    void setTabWidgets(QTabWidget* lhs, QTabWidget* rhs) noexcept;
    QTreeWidget* getSelectionTreeWidget() const;
private:
    //std::unique_ptr<vrsa::calculations::MapCalculator> mMapCalculator;
    vrsa::gdalwrapper::SpatialReference mProjCrs;

    MapHolder* mMapView;
    TreeWidget* mTreeWidget;
    graphics::MapScene* mMapScene;
    QTabWidget* mRightTabWidget;
    QTabWidget* mLeftTabWidget;

    std::unique_ptr<DigitizingManager> mDigitizingManager;
    graphics::SymbolRenderer mRenderer;
    const int DATA_COLUMN = 0;

    const int SELECTION_TAB_INDEX = 1;
    const QString SELECTION_TREE_OBJ_NAME = "treeWidgetSelection";
    //std::unique_ptr<vrsa::georef::SpatialReferenceLibrary> mCRSLib;

public slots:
    void showContextMenu(const QPoint&);

    //слоты для работы с основной легендой
    void layerTreeDataChanged(QTreeWidgetItem*, int);
    void onLayerTreeItemDoubleClicked(QTreeWidgetItem*, int);
    void onItemDragRequested(QDragMoveEvent* event, bool* accepted);
    void onItemDropped(QDropEvent* event, bool* accepted);
    //обработка кликов с mainwindow
    void onSingleSelectionToolClicked(bool checked);
    void onRectSelectionToolClicked(bool checked);

    //обработка всех событий с инструментов карты
    void onToolEvent(tools::MapTool::ToolEventType type, const QVariant& data); //вся информация с инструментов
    //специфичные методы для обработки данных с инструментов
    void handleFeatureSelected(graphics::FeatureGraphicsItem* item, bool shouldClearTree);
    void handleMultipleFeaturesSelected(const std::vector<graphics::FeatureGraphicsItem *> &items);

signals:
    void DatasetAdded(std::string src);
    void ProjectCRSChanged(vrsa::gdalwrapper::SpatialReference &ref);
    void updateLegendIconsRequired(graphics::Symbol* symbol, QIcon &icon);

    void activeLayerChanged(vrsa::vector::VectorLayer*);
    void activeLayerChanged(const QIcon&); //для изменения иконке в mainwindow для инструментов работы с активным слоем
    void activeLayerChanged(const QString&); //для изменения лейбла в mainwindow



};
}
}

#endif // GISCONTROLLER_H
