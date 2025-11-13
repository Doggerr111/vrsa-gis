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
namespace vrsa
{
namespace services
{


class GISController : public QObject
{
    Q_OBJECT
public:
    explicit GISController(QObject *parent = nullptr);

    void initializeScene(MapHolder* view);

    void LoadDataSet(std::string& source);

    void ApplyCRS(std::string name);

    bool isCurrentCRSGeographic() const;

    inline graphics::MapScene* getScene() const
    {
        return mMapScene;
    }


private:
    std::unique_ptr<vrsa::services::ProjectManager> mProjManager;
    //std::unique_ptr<vrsa::calculations::MapCalculator> mMapCalculator;
    vrsa::gdalwrapper::SpatialReference mProjCrs;
    MapHolder* mMapView;
    graphics::MapScene* mMapScene;
    //std::unique_ptr<vrsa::georef::SpatialReferenceLibrary> mCRSLib;

signals:
    void DatasetAdded(std::string src);
    void ProjectCRSChanged(vrsa::gdalwrapper::SpatialReference &ref);

};
}
}

#endif // GISCONTROLLER_H
