#include "maptoolfactory.h"
#include "tools/singleselectiontool.h"
#include "tools/rectselectiontool.h"
#include "tools/editgeometrytool.h"
#include "tools/pointdigitizingtool.h"
#include "tools/polygondigitizingtool.h"
#include "tools/linedigitizingtool.h"
#include "QDebug"
#include "vector/vectorlayer.h"
std::unique_ptr<vrsa::tools::MapTool>
vrsa::tools::MapToolFactory::createForScene(graphics::MapScene *scene, common::MapToolType toolType,
                                            vector::VectorLayer*activeLayer)
{

    switch (toolType)
    {
    case common::MapToolType::SingleSelectionTool:
        qDebug()<<"factory single";
        return std::make_unique<tools::SingleSelectionTool>(scene);
    case common::MapToolType::RectSelectionTool:
        qDebug()<<"factory rect";
        return std::make_unique<tools::RectSelectionTool>(scene);
    case common::MapToolType::EditGeometryTool:
        qDebug()<<"factory rect";
        return std::make_unique<tools::EditGeometryTool>(scene);
        //digitizing
    case common::MapToolType::PointDigitizingTool:
    {
        qDebug()<<"PointDigitizingTool";
        if (!activeLayer)
            return {};
        assert(activeLayer->getGeomType() == common::GeometryType::Point);
        return std::make_unique<tools::PointDigitizingTool>(scene, activeLayer);
    }
    case common::MapToolType::LineStringDigitizingTool:
    {
        qDebug()<<"LineDigitizingTool";
        if (!activeLayer)
            return {};
        assert(activeLayer->getGeomType() == common::GeometryType::LineString);
        return std::make_unique<tools::LineDigitizingTool>(scene, activeLayer);
    }
    case common::MapToolType::PolygonDigitizingTool:
    {
        qDebug()<<"PolyDigitizingTool";
        if (!activeLayer)
            return {};
        assert(activeLayer->getGeomType() == common::GeometryType::Polygon);
        return std::make_unique<tools::PolygonDigitizingTool>(scene, activeLayer);
    }


    default:
        return nullptr;
    }
    return nullptr;


}
