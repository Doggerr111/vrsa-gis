#include "maptool.h"
#include "graphics/mapscene.h"
vrsa::tools::MapTool::MapTool(graphics::MapScene *scene, QObject *parent)
    : QObject{parent},
      mMapScene{scene}
{

}
