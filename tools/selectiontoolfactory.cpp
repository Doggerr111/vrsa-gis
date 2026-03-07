#include "selectiontoolfactory.h"
#include "tools/singleselectiontool.h"
#include "tools/rectselectiontool.h"
#include "QDebug"
std::unique_ptr<vrsa::tools::SelectionTool>
vrsa::tools::SelectionToolFactory::createForScene(graphics::MapScene *scene, common::MapToolType toolType)
{

    switch (toolType)
    {
    case common::MapToolType::SingleSelectionTool:
        qDebug()<<"factory single";
        return std::make_unique<tools::SingleSelectionTool>(scene);
    case common::MapToolType::RectSelectionTool:
        qDebug()<<"factory rect";
        return std::make_unique<tools::RectSelectionTool>(scene);

    default:
        return nullptr;
    }
    return nullptr;


}
