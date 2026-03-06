#include "selectiontoolfactory.h"
#include "tools/singleselectiontool.h"

std::unique_ptr<vrsa::tools::SelectionTool>
vrsa::tools::SelectionToolFactory::createForScene(graphics::MapScene *scene, common::MapToolType toolType)
{

    switch (toolType)
    {
    case common::MapToolType::SingleSelectionTool:
        return std::make_unique<tools::SingleSelectionTool>(scene);

    default:
        return nullptr;
    }
    return nullptr;


}
