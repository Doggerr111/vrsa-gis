#include "maptoolfactory.h"
#include "tools/singleselectiontool.h"
#include "tools/rectselectiontool.h"
#include "tools/editgeometrytool.h"
#include "tools/pointdigitizingtool.h"
#include "tools/polygondigitizingtool.h"
#include "tools/linedigitizingtool.h"
#include "tools/rulertool.h"
#include "QDebug"
#include "vector/vectorlayer.h"
std::unique_ptr<vrsa::tools::MapTool>
vrsa::tools::MapToolFactory::createForScene(graphics::MapScene *scene, common::MapToolType toolType,
                                            vector::VectorLayer*activeLayer)
{
    switch (toolType)
    {
    case common::MapToolType::SingleSelectionTool:
        return std::make_unique<tools::SingleSelectionTool>(scene);
    case common::MapToolType::RectSelectionTool:
        return std::make_unique<tools::RectSelectionTool>(scene);
    case common::MapToolType::EditGeometryTool:
        return std::make_unique<tools::EditGeometryTool>(scene);
    case common::MapToolType::RulerTool:
        return std::make_unique<tools::RulerTool>(scene);
    case common::MapToolType::PointDigitizingTool:
        if (!activeLayer) return nullptr;
        assert(activeLayer->getGeomType() == common::GeometryType::Point);
        return std::make_unique<tools::PointDigitizingTool>(scene, activeLayer);
    case common::MapToolType::LineStringDigitizingTool:
        if (!activeLayer) return nullptr;
        assert(activeLayer->getGeomType() == common::GeometryType::LineString);
        return std::make_unique<tools::LineDigitizingTool>(scene, activeLayer);
    case common::MapToolType::PolygonDigitizingTool:
        if (!activeLayer) return nullptr;
        assert(activeLayer->getGeomType() == common::GeometryType::Polygon);
        return std::make_unique<tools::PolygonDigitizingTool>(scene, activeLayer);

    default:
        return nullptr;
    }
}
