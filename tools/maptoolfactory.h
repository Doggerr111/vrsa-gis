#ifndef MAPTOOLFACTORY_H
#define MAPTOOLFACTORY_H
#include "tools/maptool.h"

namespace vrsa
{
//namespace vector
//{
//class VectorLayer;
//}
namespace graphics
{
class MapScene;
}
namespace tools
{


class MapToolFactory
{
public:
    MapToolFactory() = delete;
    static std::unique_ptr<MapTool> createForScene(graphics::MapScene* scene, common::MapToolType, vector::VectorLayer* activeLayer = nullptr);
};

}
}

class MapToolFactory
{
public:
    MapToolFactory();
};

#endif // MAPTOOLFACTORY_H
