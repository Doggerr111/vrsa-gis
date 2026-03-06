#ifndef DIGITIZINGTOOLFACTORY_H
#define DIGITIZINGTOOLFACTORY_H
#include "tools/digitizingtool.h"

namespace vrsa
{
namespace graphics
{
class MapScene;
}
namespace tools
{


class DigitizingToolFactory
{
public:
    DigitizingToolFactory() = delete;
    static std::unique_ptr<DigitizingTool> createForScene(graphics::MapScene* scene, common::GeometryType geomType);
};

}
}

#endif // DIGITIZINGTOOLFACTORY_H
