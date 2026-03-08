#ifndef MAPTOOLFACTORY_H
#define MAPTOOLFACTORY_H
#include "tools/maptool.h"

namespace vrsa
{
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
    static std::unique_ptr<MapTool> createForScene(graphics::MapScene* scene, common::MapToolType);
};

}
}

class MapToolFactory
{
public:
    MapToolFactory();
};

#endif // MAPTOOLFACTORY_H
