#ifndef GISCONTROLLER_H
#define GISCONTROLLER_H

#include <QObject>
#include <QGraphicsView>
#include "graphics/mapscene.h"
#include "common/logger.h"
#include "services/projectmanager.h"
#include "gdal/gdalreader.h"
namespace vrsa
{
namespace services
{


class GISController : public QObject
{
    Q_OBJECT
public:
    explicit GISController(QObject *parent = nullptr);

    void initializeScene(QGraphicsView* view);

    void LoadDataSet(std::string& source);



private:
    std::unique_ptr<vrsa::services::ProjectManager> mProjManager;

signals:
    void DatasetAdded(std::string src);

};
}
}

#endif // GISCONTROLLER_H
