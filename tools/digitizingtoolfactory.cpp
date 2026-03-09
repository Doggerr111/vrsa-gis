#include "digitizingtoolfactory.h"
#include "pointdigitizingtool.h"
#include "linedigitizingtool.h"
#include "polygondigitizingtool.h"


std::unique_ptr<vrsa::tools::DigitizingTool> vrsa::tools::DigitizingToolFactory::createForScene(graphics::MapScene* scene, common::GeometryType geomType)
{
//    switch (geomType)
//    {
//    case common::GeometryType::Point:
//    case common::GeometryType::MultiPoint:
//    {
//        return std::make_unique<tools::PointDigitizingTool>(scene);
//    }
//    case common::GeometryType::LineString:
//    case common::GeometryType::MultiLineString:
//    {
//        return std::make_unique<tools::LineDigitizingTool>(scene);
//    }
//    case common::GeometryType::Polygon:
//    case common::GeometryType::MultiPolygon:
//    {
//        return std::make_unique<tools::PolygonDigitizingTool>(scene);
//    }
//    default:
//        return nullptr;
//    }
//    return nullptr;
}
