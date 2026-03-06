#ifndef SELECTIONTOOLFACTORY_H
#define SELECTIONTOOLFACTORY_H
#include "tools/selectiontool.h"

namespace vrsa
{
namespace graphics
{
class MapScene;
}
namespace tools
{


class SelectionToolFactory
{
public:
    SelectionToolFactory() = delete;
    static std::unique_ptr<SelectionTool> createForScene(graphics::MapScene* scene, common::MapToolType);
};

}
}

class SelectionToolFactory
{
public:
    SelectionToolFactory();
};

#endif // SELECTIONTOOLFACTORY_H
